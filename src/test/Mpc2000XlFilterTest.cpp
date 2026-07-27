#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "engine/Mpc2000XlFilterEnvelope.hpp"
#include "engine/filter/StateVariableFilter.hpp"
#include "performance/ProgramMapper.hpp"
#include "sampler/NoteParameters.hpp"

#include <array>
#include <functional>

using Catch::Approx;
using namespace mpc::engine;
using namespace mpc::engine::filter;

TEST_CASE("MPC2000XL filter uses the firmware cutoff table",
          "[filter][mpc2000xl]")
{
    constexpr std::array<int, 101> expected{
        164,   172,   180,   188,   197,   206,   216,   226,   237,   248,
        260,   272,   285,   298,   312,   327,   342,   358,   375,   393,
        412,   431,   451,   472,   495,   518,   542,   568,   595,   623,
        652,   683,   715,   749,   784,   821,   860,   900,   943,   987,
        1034,  1082,  1133,  1187,  1243,  1301,  1363,  1427,  1494,  1565,
        1638,  1716,  1796,  1881,  1970,  2062,  2160,  2261,  2368,  2480,
        2597,  2719,  2847,  2981,  3122,  3269,  3423,  3584,  3753,  3930,
        4115,  4309,  4512,  4725,  4948,  5181,  5425,  5681,  5948,  6229,
        6522,  6830,  7151,  7488,  7841,  8211,  8598,  9003,  9427,  9872,
        10337, 10824, 11334, 11868, 12428, 13013, 13627, 14269, 14941, 15646,
        16383};

    for (int i = 0; i < static_cast<int>(expected.size()); ++i)
    {
        REQUIRE(Mpc2000XlFilterEnvelope::cutoffTableValue(i) == expected[i]);
    }
}

TEST_CASE("MPC2000XL filter derives firmware register values",
          "[filter][mpc2000xl]")
{
    Mpc2000XlFilterEnvelope envelope;
    Mpc2000XlFilterEnvelope::Parameters parameters;
    parameters.cutoff = 20;
    parameters.resonance = 7;
    parameters.attack = 50;
    parameters.decay = 75;
    parameters.amount = 60;
    envelope.configure(parameters);

    REQUIRE(envelope.getBaseIndex() == 20);
    REQUIRE(envelope.getTargetIndex() == 80);
    REQUIRE(envelope.getAttackRate() == 26);
    REQUIRE(envelope.getDecayRate() == -11);
    REQUIRE(envelope.getAttackDurationMs() == 1250);
    REQUIRE(envelope.getDecayDurationMs() == 2813);
    REQUIRE(envelope.getDecayTransitionTicks() == 127);
    REQUIRE(envelope.getCurrentHardwareCoefficient() ==
            Approx((2.0 * 0x019c + 1.0) / (1 << 15)));
    REQUIRE(envelope.getAttackTargetHardwareCoefficient() ==
            Approx(0x32f0 / static_cast<double>(1 << 15)));
    REQUIRE(envelope.getDecayTargetHardwareCoefficient() ==
            Approx(0x0330 / static_cast<double>(1 << 15)));
}

TEST_CASE("MPC2000XL filter combines velocity and note variation as offsets",
          "[filter][mpc2000xl]")
{
    Mpc2000XlFilterEnvelope envelope;
    Mpc2000XlFilterEnvelope::Parameters parameters;
    parameters.cutoff = 20;
    parameters.velocity = 64;
    parameters.velocityToCutoff = 100;
    parameters.filterNoteVariationEnabled = true;
    parameters.filterNoteVariation = 25;
    envelope.configure(parameters);
    REQUIRE(envelope.getBaseIndex() == 45);

    parameters.filterNoteVariation = 0;
    parameters.cutoff = 0;
    parameters.velocityToCutoff = 0;
    envelope.configure(parameters);
    REQUIRE(envelope.getBaseIndex() == 0);

    parameters.filterNoteVariation = 100;
    parameters.cutoff = 100;
    envelope.configure(parameters);
    REQUIRE(envelope.getBaseIndex() == 100);
}

TEST_CASE("MPC2000XL filter time curve and resonance cover their full ranges",
          "[filter][mpc2000xl]")
{
    REQUIRE(Mpc2000XlFilterEnvelope::durationMs(0) == 0);
    REQUIRE(Mpc2000XlFilterEnvelope::durationMs(50) == 1250);
    REQUIRE(Mpc2000XlFilterEnvelope::durationMs(100) == 5000);

    for (int resonance = 0; resonance <= 15; ++resonance)
    {
        REQUIRE(Mpc2000XlFilterEnvelope::resonanceDamping(resonance) ==
                Approx((16.0 - resonance) / 16.0));
    }
}

TEST_CASE("MPC2000XL filter coefficient ramp is multiplicative",
          "[filter][mpc2000xl]")
{
    Mpc2000XlFilterEnvelope envelope;
    Mpc2000XlFilterEnvelope::Parameters parameters;
    parameters.cutoff = 20;
    parameters.attack = 100;
    parameters.decay = 100;
    parameters.amount = 60;
    envelope.configure(parameters);

    const auto initial = envelope.getCurrentHardwareCoefficient();
    const auto first = envelope.nextCoefficient();
    REQUIRE(first == Approx(initial * (1.0 + 6.0 / (1 << 19))).epsilon(1e-12));
}

TEST_CASE("MPC2000XL zero-amount filter remains static", "[filter][mpc2000xl]")
{
    Mpc2000XlFilterEnvelope envelope;
    Mpc2000XlFilterEnvelope::Parameters parameters;
    parameters.cutoff = 50;
    parameters.attack = 100;
    parameters.decay = 100;
    parameters.amount = 0;
    envelope.configure(parameters);

    const auto initial = envelope.getCurrentHardwareCoefficient();
    for (int i = 0; i < 1000; ++i)
    {
        envelope.nextCoefficient();
    }

    REQUIRE(envelope.getAttackRate() == 0);
    REQUIRE(envelope.getDecayRate() == 0);
    REQUIRE(envelope.getAttackDurationMs() == 0);
    REQUIRE(envelope.getDecayDurationMs() == 0);
    REQUIRE(envelope.getCurrentHardwareCoefficient() == initial);
}

TEST_CASE("MPC2000XL filter decay starts on the firmware scheduler boundary",
          "[filter][mpc2000xl]")
{
    Mpc2000XlFilterEnvelope envelope;
    Mpc2000XlFilterEnvelope::Parameters parameters;
    parameters.cutoff = 20;
    parameters.attack = 0;
    parameters.decay = 100;
    parameters.amount = 60;
    envelope.configure(parameters);

    REQUIRE(envelope.getDecayTransitionTicks() == 2);
    REQUIRE(envelope.getDecayRate() == -6);

    for (int i = 0; i < 2 * 441; ++i)
    {
        envelope.nextCoefficient();
    }

    REQUIRE(envelope.getCurrentHardwareCoefficient() ==
            Approx(envelope.getAttackTargetHardwareCoefficient()));
    envelope.nextCoefficient();
    REQUIRE(envelope.getCurrentHardwareCoefficient() <
            envelope.getAttackTargetHardwareCoefficient());
}

TEST_CASE("MPC2000XL low-pass uses one band-first Chamberlin update",
          "[filter][mpc2000xl]")
{
    const auto coefficient =
        Mpc2000XlFilterEnvelope::initialCoefficientForIndex(50);
    StateVariableFilter filter;

    const auto first = filter.filter(1.0f, coefficient, 0);
    REQUIRE(first == Approx(coefficient * coefficient).epsilon(1e-7));

    const auto second = filter.filter(0.0f, coefficient, 0);
    const auto expected = 2.0 * coefficient * coefficient -
                          coefficient * coefficient * coefficient -
                          coefficient * coefficient * coefficient * coefficient;
    REQUIRE(second == Approx(expected).epsilon(1e-7));

    filter.resetElementState();
    const auto resonant = filter.filter(1.0f, coefficient, 15);
    REQUIRE(resonant == Approx(first).epsilon(1e-7));
}

TEST_CASE("Performance mapper preserves every filter envelope parameter",
          "[filter][mpc2000xl]")
{
    mpc::performance::NoteParameters source;
    source.filterAttack = 17;
    source.filterDecay = 38;
    source.filterEnvelopeAmount = 79;

    mpc::sampler::GetNoteParametersFn getSnapshot(
        [&source]
        {
            return source;
        });
    const std::function<mpc::ProgramIndex()> getProgramIndex(
        []
        {
            return mpc::ProgramIndex(0);
        });
    const std::function<void(mpc::performance::PerformanceMessage &&)> dispatch(
        [](mpc::performance::PerformanceMessage &&) {});
    const mpc::sampler::NoteParameters samplerParameters(0, getProgramIndex,
                                                         getSnapshot, dispatch);

    const auto mapped =
        mpc::performance::mapSamplerNoteParametersToPerformanceNoteParameters(
            &samplerParameters);
    REQUIRE(mapped.filterAttack == 17);
    REQUIRE(mapped.filterDecay == 38);
    REQUIRE(mapped.filterEnvelopeAmount == 79);
}
