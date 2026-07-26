#include "Mpc2000XlAmpEnvelope.hpp"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <limits>
#include <stdexcept>

using namespace mpc::engine;

namespace {
constexpr int64_t kQ32One = int64_t{1} << 32;
}

int Mpc2000XlAmpEnvelope::truncDiv(const int64_t numerator,
                                   const int64_t denominator) {
  if (denominator == 0) {
    throw std::invalid_argument("division by zero");
  }

  const auto quotient = std::llabs(numerator) / std::llabs(denominator);
  return static_cast<int>((numerator < 0) != (denominator < 0) ? -quotient
                                                               : quotient);
}

int Mpc2000XlAmpEnvelope::roundedRatio(const int64_t numerator,
                                       const int64_t denominator) {
  return truncDiv(numerator + denominator / 2, denominator);
}

int Mpc2000XlAmpEnvelope::userCurve(const int value) {
  if (value < 0 || value > 100) {
    throw std::out_of_range("envelope value");
  }

  return (value * value + 1) / 2;
}

int Mpc2000XlAmpEnvelope::startOffsetFrames(const int velocity,
                                            const int velocityToStart) {
  const auto velocityStart = truncDiv((128 - velocity) * velocityToStart, 127);
  return truncDiv(441 * userCurve(velocityStart), 10);
}

int Mpc2000XlAmpEnvelope::ordinaryRampFrames(const int distance,
                                             const int rateMagnitude) {
  return (distance * 8 + rateMagnitude - 1) / rateMagnitude;
}

int Mpc2000XlAmpEnvelope::rateForMs(const int target, const int pitchStep,
                                    const int milliseconds) {
  return roundedRatio(static_cast<int64_t>(target) * pitchStep,
                      static_cast<int64_t>(kRateDenominator) * milliseconds);
}

int Mpc2000XlAmpEnvelope::durationMs(const int target,
                                     const int rateMagnitude) {
  return truncDiv(truncDiv(2 * target, rateMagnitude), 11);
}

void Mpc2000XlAmpEnvelope::configure(const Parameters &newParameters) {
  parameters = newParameters;
  setOutputSampleRate(parameters.outputSampleRate);

  stage = Stage::COMPLETE;
  target = 0;
  attackRate = 0;
  decayRateMagnitude = 0;
  attackDuration = 0;
  decayDuration = 0;
  playableDuration = 0;
  transitionTicks = 0;
  forcedStopTicks = 0;
  currentRate = 0;
  levelEighths = 0;
  rateRemainderQ32 = 0;
  elapsedHardwareFramesQ32 = 0;
  shutdownStartHardwareFrameQ32 = 0;
  shutdownStartLevelEighths = 0;

  const auto velocityLevel = truncDiv(
      12700 + (parameters.velocity - 127) * parameters.velocityToLevel, 100);
  target = truncDiv(
      static_cast<int64_t>(velocityLevel) * parameters.soundLevel << 16, 50801);

  if (target <= 0) {
    return;
  }

  const auto velocityAttack =
      truncDiv((128 - parameters.velocity) * parameters.velocityToAttack, 127);
  const auto attackBasis =
      userCurve(parameters.attack) + userCurve(velocityAttack);
  attackRate =
      attackBasis == 0
          ? target
          : std::max(1, rateForMs(target, parameters.pitchStep, attackBasis));
  attackRate = std::min(attackRate, target);

  const auto decayBasis = userCurve(parameters.decay);
  decayRateMagnitude =
      decayBasis == 0
          ? 0x7fff
          : std::max(1, rateForMs(target, parameters.pitchStep, decayBasis));

  attackDuration = durationMs(target, attackRate);
  decayDuration = durationMs(target, decayRateMagnitude);

  if (parameters.looping) {
    playableDuration = std::numeric_limits<int>::max();
  } else {
    const auto remainingFrames =
        parameters.endFrame - parameters.startFrame -
        startOffsetFrames(parameters.velocity, parameters.velocityToStart);
    auto rawMs = truncDiv(static_cast<int64_t>(remainingFrames) * 10, 441);
    rawMs -= 30;
    playableDuration =
        truncDiv(static_cast<int64_t>(rawMs) << 12, parameters.pitchStep);

    if (playableDuration <= 0) {
      stage = Stage::COMPLETE;
      return;
    }

    if (decayDuration > playableDuration) {
      attackDuration = 0;
      attackRate = target;
      decayDuration = playableDuration;
      decayRateMagnitude = std::max(
          1, rateForMs(target, parameters.pitchStep, playableDuration));
    } else if (attackDuration + decayDuration > playableDuration) {
      attackDuration = playableDuration - decayDuration;
      decayRateMagnitude = std::max(1, rateForMs(target, parameters.pitchStep,
                                                 std::max(1, attackDuration)));
    }
  }

  if (parameters.decayModeStart) {
    transitionTicks = std::max(2, (attackDuration + 5) / 10);
    forcedStopTicks = std::max(1, (attackDuration + decayDuration + 5) / 10);
  } else if (parameters.looping) {
    transitionTicks = 0xffff;
    forcedStopTicks = 0xffff;
  } else {
    transitionTicks = std::max(2, (playableDuration - decayDuration + 5) / 10);
    forcedStopTicks = std::max(1, (playableDuration + 5) / 10);
  }

  currentRate = attackRate;
  stage = Stage::ATTACK;
}

void Mpc2000XlAmpEnvelope::setOutputSampleRate(const double outputSampleRate) {
  if (outputSampleRate <= 0) {
    throw std::out_of_range("output sample rate");
  }

  parameters.outputSampleRate = outputSampleRate;
  hardwareFramesPerOutputFrameQ32 = static_cast<int64_t>(
      std::llround(kHardwareSampleRate / outputSampleRate * kQ32One));
}

void Mpc2000XlAmpEnvelope::beginDecay() {
  stage = Stage::DECAY;
  currentRate = -decayRateMagnitude;
}

void Mpc2000XlAmpEnvelope::beginShutdown() {
  if (stage == Stage::COMPLETE || stage == Stage::SHUTDOWN) {
    return;
  }

  stage = Stage::SHUTDOWN;
  shutdownStartHardwareFrameQ32 = elapsedHardwareFramesQ32;
  shutdownStartLevelEighths = static_cast<int>(levelEighths);
}

void Mpc2000XlAmpEnvelope::advanceLinearRamp() {
  rateRemainderQ32 +=
      static_cast<int64_t>(currentRate) * hardwareFramesPerOutputFrameQ32;
  const auto delta = truncDiv(rateRemainderQ32, kQ32One);
  rateRemainderQ32 -= static_cast<int64_t>(delta) * kQ32One;
  levelEighths += delta;

  const auto targetEighths =
      static_cast<int64_t>(stage == Stage::ATTACK ? target : 0) * 8;
  if (currentRate >= 0) {
    levelEighths = std::min(levelEighths, targetEighths);
  } else {
    levelEighths = std::max(levelEighths, targetEighths);
  }
}

float Mpc2000XlAmpEnvelope::shutdownMultiplier() const {
  // The firmware requests the DSP's anti-click shutdown here. Its internal
  // curve is unavailable, so retain the measured four-frame drop and tail.
  const auto elapsed = static_cast<double>(elapsedHardwareFramesQ32 -
                                           shutdownStartHardwareFrameQ32) /
                       kQ32One;
  if (elapsed <= kShutdownLinearFrames) {
    return static_cast<float>(1.0 - 0.84 * elapsed / kShutdownLinearFrames);
  }

  const auto tailFrame = elapsed - kShutdownLinearFrames;
  if (tailFrame >= kShutdownTailFrames) {
    return 0;
  }

  return static_cast<float>(
      0.16 * std::exp(std::log(1e-5) * tailFrame / kShutdownTailFrames));
}

int64_t Mpc2000XlAmpEnvelope::ticksToHardwareFramesQ32(const int ticks) const {
  return static_cast<int64_t>(ticks) * 441 * kQ32One;
}

float Mpc2000XlAmpEnvelope::next() {
  if (stage == Stage::COMPLETE) {
    return 0;
  }

  // Hardware processes these timers on a global 10 ms scheduler. Voice owns
  // no global scheduler phase, so ticks are measured from key-on here.
  if (stage != Stage::SHUTDOWN) {
    if (forcedStopTicks != 0xffff &&
        elapsedHardwareFramesQ32 >=
            ticksToHardwareFramesQ32(forcedStopTicks)) {
      beginShutdown();
    } else if (transitionTicks != 0xffff && stage == Stage::ATTACK &&
               elapsedHardwareFramesQ32 >=
                   ticksToHardwareFramesQ32(transitionTicks)) {
      beginDecay();
    }
  }

  float result = 0;
  if (stage == Stage::SHUTDOWN) {
    const auto multiplier = shutdownMultiplier();
    result = static_cast<float>(shutdownStartLevelEighths) /
             (kFullScaleTarget * 8) * multiplier;
    if (multiplier == 0) {
      stage = Stage::COMPLETE;
    }
  } else {
    advanceLinearRamp();
    result = static_cast<float>(levelEighths) / (kFullScaleTarget * 8);
  }

  elapsedHardwareFramesQ32 += hardwareFramesPerOutputFrameQ32;
  return result;
}

void Mpc2000XlAmpEnvelope::startRelease() {
  if (stage == Stage::COMPLETE) {
    return;
  }

  if (parameters.looping && !parameters.decayModeStart) {
    transitionTicks = 1;
    forcedStopTicks = std::max(2, (decayDuration + 5) / 10);
    elapsedHardwareFramesQ32 = 0;
    rateRemainderQ32 = 0;
    stage = Stage::ATTACK;
    return;
  }

  beginShutdown();
}

bool Mpc2000XlAmpEnvelope::isComplete() const {
  return stage == Stage::COMPLETE;
}

Mpc2000XlAmpEnvelope::Stage Mpc2000XlAmpEnvelope::getStage() const {
  return stage;
}

int Mpc2000XlAmpEnvelope::getTarget() const { return target; }

int Mpc2000XlAmpEnvelope::getAttackRate() const { return attackRate; }

int Mpc2000XlAmpEnvelope::getDecayRateMagnitude() const {
  return decayRateMagnitude;
}

int Mpc2000XlAmpEnvelope::getAttackDurationMs() const { return attackDuration; }

int Mpc2000XlAmpEnvelope::getDecayDurationMs() const { return decayDuration; }

int Mpc2000XlAmpEnvelope::getPlayableDurationMs() const {
  return playableDuration;
}

int Mpc2000XlAmpEnvelope::getTransitionTicks() const { return transitionTicks; }

int Mpc2000XlAmpEnvelope::getForcedStopTicks() const { return forcedStopTicks; }
