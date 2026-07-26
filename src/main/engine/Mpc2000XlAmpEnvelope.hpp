#pragma once

#include <cstdint>

namespace mpc::engine {

// Models the MPC2000XL OS 1.20 envelope setup and firmware-side timing.
class Mpc2000XlAmpEnvelope {
public:
  struct Parameters {
    int attack = 0;
    int decay = 0;
    int velocity = 127;
    int velocityToLevel = 100;
    int velocityToAttack = 0;
    int velocityToStart = 0;
    int soundLevel = 100;
    int pitchStep = 0x1000;
    int startFrame = 0;
    int endFrame = 0;
    bool looping = false;
    bool decayModeStart = false;
    double outputSampleRate = 44100.0;
  };

  enum class Stage { ATTACK, DECAY, SHUTDOWN, COMPLETE };

  void configure(const Parameters &);
  void setOutputSampleRate(double);
  float next();
  void startRelease();

  bool isComplete() const;
  Stage getStage() const;

  int getTarget() const;
  int getAttackRate() const;
  int getDecayRateMagnitude() const;
  int getAttackDurationMs() const;
  int getDecayDurationMs() const;
  int getPlayableDurationMs() const;
  int getTransitionTicks() const;
  int getForcedStopTicks() const;

  static int userCurve(int);
  static int startOffsetFrames(int velocity, int velocityToStart);
  static int ordinaryRampFrames(int distance, int rateMagnitude);

private:
  static constexpr int kHardwareSampleRate = 44100;
  static constexpr int kRateDenominator = 0x57db;
  static constexpr int kFullScaleTarget = 0x3fff;
  static constexpr int kShutdownLinearFrames = 3;
  static constexpr int kShutdownTailFrames = 1059;

  static int truncDiv(int64_t, int64_t);
  static int roundedRatio(int64_t, int64_t);
  static int rateForMs(int target, int pitchStep, int milliseconds);
  static int durationMs(int target, int rateMagnitude);

  void beginDecay();
  void beginShutdown();
  void advanceLinearRamp();
  float shutdownMultiplier() const;
  int64_t ticksToHardwareFramesQ32(int) const;

  Parameters parameters;
  Stage stage = Stage::COMPLETE;
  int target = 0;
  int attackRate = 0;
  int decayRateMagnitude = 0;
  int attackDuration = 0;
  int decayDuration = 0;
  int playableDuration = 0;
  int transitionTicks = 0;
  int forcedStopTicks = 0;
  int currentRate = 0;
  int64_t levelEighths = 0;
  int64_t rateRemainderQ32 = 0;
  int64_t elapsedHardwareFramesQ32 = 0;
  int64_t hardwareFramesPerOutputFrameQ32 = int64_t{1} << 32;
  int64_t shutdownStartHardwareFrameQ32 = 0;
  int shutdownStartLevelEighths = 0;
};

} // namespace mpc::engine
