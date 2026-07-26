#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "engine/Mpc2000XlAmpEnvelope.hpp"

#include <algorithm>
#include <array>

using mpc::engine::Mpc2000XlAmpEnvelope;

namespace {
Mpc2000XlAmpEnvelope::Parameters capturedParameters() {
  Mpc2000XlAmpEnvelope::Parameters result;
  result.attack = 100;
  result.decay = 100;
  result.endFrame = 159554;
  return result;
}
} // namespace

TEST_CASE("MPC2000XL envelope user curve covers every persisted value",
          "[amp-envelope]") {
  for (int value = 0; value <= 100; value++) {
    REQUIRE(Mpc2000XlAmpEnvelope::userCurve(value) == (value * value + 1) / 2);
  }
}

TEST_CASE("MPC2000XL envelope reproduces measured attack buckets",
          "[amp-envelope]") {
  constexpr std::array<std::pair<int, int>, 23> expected = {{
      {0, 8},       {1, 44},       {2, 88},      {3, 220},    {4, 352},
      {5, 570},     {10, 2185},    {20, 8738},   {24, 13107}, {25, 13107},
      {31, 21844},  {32, 21844},   {33, 26213},  {36, 26213}, {37, 32766},
      {41, 32766},  {42, 43688},   {48, 43688},  {49, 65532}, {63, 65532},
      {64, 131064}, {100, 131064}, {99, 131064},
  }};

  for (const auto &[attack, frames] : expected) {
    auto parameters = capturedParameters();
    parameters.attack = attack;
    parameters.looping = true;

    Mpc2000XlAmpEnvelope envelope;
    envelope.configure(parameters);

    REQUIRE(Mpc2000XlAmpEnvelope::ordinaryRampFrames(
                envelope.getTarget(), envelope.getAttackRate()) == frames);
  }
}

TEST_CASE("MPC2000XL envelope reproduces controlled collision anchors",
          "[amp-envelope]") {
  Mpc2000XlAmpEnvelope envelope;
  auto parameters = capturedParameters();
  parameters.decayModeStart = true;
  envelope.configure(parameters);

  REQUIRE(envelope.getTarget() == 0x3fff);
  REQUIRE(envelope.getAttackRate() == 1);
  REQUIRE(envelope.getDecayRateMagnitude() == 5);
  REQUIRE(envelope.getAttackDurationMs() == 610);
  REQUIRE(envelope.getDecayDurationMs() == 2978);
  REQUIRE(envelope.getPlayableDurationMs() == 3588);
  REQUIRE(envelope.getTransitionTicks() == 61);
  REQUIRE(envelope.getForcedStopTicks() == 359);

  parameters.attack = 0;
  envelope.configure(parameters);
  REQUIRE(envelope.getAttackRate() == 0x3fff);
  REQUIRE(envelope.getDecayRateMagnitude() == 1);
  REQUIRE(envelope.getAttackDurationMs() == 0);
  REQUIRE(envelope.getDecayDurationMs() == 2978);
  REQUIRE(envelope.getTransitionTicks() == 2);
  REQUIRE(envelope.getForcedStopTicks() == 298);

  parameters.decay = 0;
  envelope.configure(parameters);
  REQUIRE(envelope.getDecayRateMagnitude() == 0x7fff);
  REQUIRE(envelope.getTransitionTicks() == 2);
  REQUIRE(envelope.getForcedStopTicks() == 1);
}

TEST_CASE("MPC2000XL END decay starts from playable lifetime",
          "[amp-envelope]") {
  auto parameters = capturedParameters();
  parameters.attack = 0;
  parameters.decayModeStart = false;

  Mpc2000XlAmpEnvelope envelope;
  envelope.configure(parameters);

  REQUIRE(envelope.getTransitionTicks() == 61);
  REQUIRE(envelope.getForcedStopTicks() == 359);
}

TEST_CASE("MPC2000XL START decay uses the firmware scheduler boundary",
          "[amp-envelope]") {
  auto parameters = capturedParameters();
  parameters.attack = 0;
  parameters.decayModeStart = true;
  parameters.looping = true;

  Mpc2000XlAmpEnvelope envelope;
  envelope.configure(parameters);

  for (int frame = 0; frame < 2 * 441; frame++) {
    envelope.next();
  }
  REQUIRE(envelope.getStage() == Mpc2000XlAmpEnvelope::Stage::ATTACK);

  envelope.next();
  REQUIRE(envelope.getStage() == Mpc2000XlAmpEnvelope::Stage::DECAY);
}

TEST_CASE("MPC2000XL attack zero reaches target in eight hardware frames",
          "[amp-envelope]") {
  auto parameters = capturedParameters();
  parameters.attack = 0;
  parameters.looping = true;

  Mpc2000XlAmpEnvelope envelope;
  envelope.configure(parameters);

  float value = 0;
  for (int frame = 0; frame < 8; frame++) {
    value = envelope.next();
  }

  REQUIRE(value == Catch::Approx(1.0f));
}

TEST_CASE("MPC2000XL envelope applies velocity-to-start and pitch",
          "[amp-envelope]") {
  REQUIRE(Mpc2000XlAmpEnvelope::startOffsetFrames(1, 100) == 220500);

  auto parameters = capturedParameters();
  parameters.pitchStep = 0x2000;

  Mpc2000XlAmpEnvelope envelope;
  envelope.configure(parameters);

  REQUIRE(envelope.getPlayableDurationMs() == 1794);
}

TEST_CASE("MPC2000XL envelope rejects inaudible and too-short voices",
          "[amp-envelope]") {
  auto parameters = capturedParameters();
  parameters.soundLevel = 0;

  Mpc2000XlAmpEnvelope envelope;
  envelope.configure(parameters);
  REQUIRE(envelope.isComplete());

  parameters.soundLevel = 100;
  parameters.endFrame = 100;
  envelope.configure(parameters);
  REQUIRE(envelope.isComplete());
}

TEST_CASE("MPC2000XL envelope resolves every attack-decay collision",
          "[amp-envelope]") {
  Mpc2000XlAmpEnvelope envelope;
  Mpc2000XlAmpEnvelope::Parameters parameters;
  parameters.endFrame = 44100;

  for (int attack = 0; attack <= 100; attack++) {
    for (int decay = 0; decay <= 100; decay++) {
      parameters.attack = attack;
      parameters.decay = decay;

      for (const bool decayModeStart : {false, true}) {
        parameters.decayModeStart = decayModeStart;
        envelope.configure(parameters);

        CAPTURE(attack, decay, decayModeStart);
        REQUIRE_FALSE(envelope.isComplete());
        REQUIRE(envelope.getAttackDurationMs() +
                    envelope.getDecayDurationMs() <=
                envelope.getPlayableDurationMs());

        if (decayModeStart) {
          REQUIRE(envelope.getTransitionTicks() ==
                  std::max(2, (envelope.getAttackDurationMs() + 5) / 10));
          REQUIRE(envelope.getForcedStopTicks() ==
                  std::max(1, (envelope.getAttackDurationMs() +
                                   envelope.getDecayDurationMs() + 5) /
                                  10));
        } else {
          REQUIRE(envelope.getTransitionTicks() ==
                  std::max(2, (envelope.getPlayableDurationMs() -
                                   envelope.getDecayDurationMs() + 5) /
                                  10));
          REQUIRE(envelope.getForcedStopTicks() ==
                  std::max(1,
                           (envelope.getPlayableDurationMs() + 5) / 10));
        }
      }
    }
  }
}
