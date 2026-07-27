#include "Mpc2000XlFilterEnvelope.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <stdexcept>

using namespace mpc::engine;

namespace
{
    constexpr std::array<int, 101> kCutoffTable{
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

    int clampUserValue(const int value)
    {
        return std::clamp(value, 0, 100);
    }
} // namespace

void Mpc2000XlFilterEnvelope::configure(const Parameters &newParameters)
{
    parameters = newParameters;
    setOutputSampleRate(parameters.outputSampleRate);

    baseIndex = clampUserValue(parameters.cutoff) +
                std::clamp(parameters.velocity, 0, 127) *
                    clampUserValue(parameters.velocityToCutoff) / 127;

    if (parameters.filterNoteVariationEnabled)
    {
        baseIndex += clampUserValue(parameters.filterNoteVariation) - 50;
    }

    baseIndex = clampUserValue(baseIndex);
    targetIndex = clampUserValue(baseIndex + clampUserValue(parameters.amount));
    const auto effectiveAmount = targetIndex - baseIndex;

    currentCoefficient = initialCoefficientForIndex(baseIndex);
    attackTargetCoefficient = targetCoefficientForIndex(targetIndex);
    decayTargetCoefficient = targetCoefficientForIndex(baseIndex);
    elapsedHardwareFramesQ32 = 0;

    attackRate = 0;
    decayRate = 0;
    attackDuration = 0;
    decayDuration = 0;
    decayTransitionTicks = 0;
    stage = Stage::STATIC;

    if (effectiveAmount != 0)
    {
        const auto numerator = 1095 * effectiveAmount / 2;
        attackDuration = std::max(1, durationMs(parameters.attack));
        decayDuration = std::max(1, durationMs(parameters.decay));
        attackRate = std::min(0x7fff, numerator / attackDuration);
        decayRate = -std::min(0x7fff, numerator / decayDuration);
        decayTransitionTicks = attackDuration / 10 + 2;
        stage = Stage::ATTACK;
    }

    const auto hardwareFramesPerOutputFrame =
        static_cast<double>(hardwareFramesPerOutputFrameQ32) / kQ32One;
    rateFactorPerOutputFrame =
        std::pow(1.0 + attackRate / static_cast<double>(1 << 19),
                 hardwareFramesPerOutputFrame);
}

void Mpc2000XlFilterEnvelope::setOutputSampleRate(const double outputSampleRate)
{
    if (outputSampleRate <= 0)
    {
        throw std::out_of_range("output sample rate");
    }

    parameters.outputSampleRate = outputSampleRate;
    hardwareFramesPerOutputFrameQ32 = static_cast<int64_t>(
        std::llround(kHardwareSampleRate / outputSampleRate * kQ32One));

    const auto signedRate = stage == Stage::ATTACK
                                ? attackRate
                                : (stage == Stage::DECAY ? decayRate : 0);
    const auto hardwareFramesPerOutputFrame =
        static_cast<double>(hardwareFramesPerOutputFrameQ32) / kQ32One;
    rateFactorPerOutputFrame =
        std::pow(1.0 + signedRate / static_cast<double>(1 << 19),
                 hardwareFramesPerOutputFrame);
}

double Mpc2000XlFilterEnvelope::nextCoefficient()
{
    if (stage == Stage::ATTACK &&
        elapsedHardwareFramesQ32 >=
            static_cast<int64_t>(decayTransitionTicks) * 441 * kQ32One)
    {
        beginDecay();
    }

    advanceCoefficient();
    const auto result = coefficientForOutputSampleRate();
    elapsedHardwareFramesQ32 += hardwareFramesPerOutputFrameQ32;
    return result;
}

void Mpc2000XlFilterEnvelope::beginDecay()
{
    stage = Stage::DECAY;
    const auto hardwareFramesPerOutputFrame =
        static_cast<double>(hardwareFramesPerOutputFrameQ32) / kQ32One;
    rateFactorPerOutputFrame =
        std::pow(1.0 + decayRate / static_cast<double>(1 << 19),
                 hardwareFramesPerOutputFrame);
}

void Mpc2000XlFilterEnvelope::advanceCoefficient()
{
    if (stage == Stage::STATIC)
    {
        return;
    }

    currentCoefficient *= rateFactorPerOutputFrame;
    if (stage == Stage::ATTACK)
    {
        currentCoefficient =
            std::min(currentCoefficient, attackTargetCoefficient);
    }
    else
    {
        currentCoefficient =
            std::max(currentCoefficient, decayTargetCoefficient);
    }
}

double Mpc2000XlFilterEnvelope::coefficientForOutputSampleRate() const
{
    if (parameters.outputSampleRate == kHardwareSampleRate)
    {
        return currentCoefficient;
    }

    const auto hardwareAngle =
        std::asin(std::clamp(currentCoefficient * 0.5, 0.0, 1.0));
    return 2.0 * std::sin(kHardwareSampleRate / parameters.outputSampleRate *
                          hardwareAngle);
}

int Mpc2000XlFilterEnvelope::getBaseIndex() const
{
    return baseIndex;
}

int Mpc2000XlFilterEnvelope::getTargetIndex() const
{
    return targetIndex;
}

int Mpc2000XlFilterEnvelope::getAttackRate() const
{
    return attackRate;
}

int Mpc2000XlFilterEnvelope::getDecayRate() const
{
    return decayRate;
}

int Mpc2000XlFilterEnvelope::getAttackDurationMs() const
{
    return attackDuration;
}

int Mpc2000XlFilterEnvelope::getDecayDurationMs() const
{
    return decayDuration;
}

int Mpc2000XlFilterEnvelope::getDecayTransitionTicks() const
{
    return decayTransitionTicks;
}

double Mpc2000XlFilterEnvelope::getCurrentHardwareCoefficient() const
{
    return currentCoefficient;
}

double Mpc2000XlFilterEnvelope::getAttackTargetHardwareCoefficient() const
{
    return attackTargetCoefficient;
}

double Mpc2000XlFilterEnvelope::getDecayTargetHardwareCoefficient() const
{
    return decayTargetCoefficient;
}

int Mpc2000XlFilterEnvelope::cutoffTableValue(const int index)
{
    return kCutoffTable[clampUserValue(index)];
}

int Mpc2000XlFilterEnvelope::durationMs(const int value)
{
    const auto clamped = clampUserValue(value);
    return (clamped * clamped + 1) / 2;
}

double Mpc2000XlFilterEnvelope::initialCoefficientForIndex(const int index)
{
    return (2.0 * cutoffTableValue(index) + 1.0) / (1 << 15);
}

double Mpc2000XlFilterEnvelope::targetCoefficientForIndex(const int index)
{
    return ((cutoffTableValue(index) & 0x3ff8) << 1) /
           static_cast<double>(1 << 15);
}

double Mpc2000XlFilterEnvelope::resonanceDamping(const int resonance)
{
    return (16 - std::clamp(resonance, 0, 15)) / 16.0;
}
