#pragma once

#include <cstdint>

namespace mpc::engine
{
    class Mpc2000XlFilterEnvelope
    {
    public:
        struct Parameters
        {
            int cutoff = 100;
            int resonance = 0;
            int attack = 0;
            int decay = 0;
            int amount = 0;
            int velocityToCutoff = 0;
            int velocity = 127;
            bool filterNoteVariationEnabled = false;
            int filterNoteVariation = 50;
            double outputSampleRate = 44100.0;
        };

        void configure(const Parameters &);
        void setOutputSampleRate(double);
        double nextCoefficient();

        int getBaseIndex() const;
        int getTargetIndex() const;
        int getAttackRate() const;
        int getDecayRate() const;
        int getAttackDurationMs() const;
        int getDecayDurationMs() const;
        int getDecayTransitionTicks() const;
        double getCurrentHardwareCoefficient() const;
        double getAttackTargetHardwareCoefficient() const;
        double getDecayTargetHardwareCoefficient() const;

        static int cutoffTableValue(int);
        static int durationMs(int);
        static double initialCoefficientForIndex(int);
        static double targetCoefficientForIndex(int);
        static double resonanceDamping(int);

    private:
        enum class Stage
        {
            STATIC,
            ATTACK,
            DECAY
        };

        static constexpr int kHardwareSampleRate = 44100;
        static constexpr int64_t kQ32One = int64_t{1} << 32;

        void beginDecay();
        void advanceCoefficient();
        double coefficientForOutputSampleRate() const;

        Parameters parameters;
        Stage stage = Stage::STATIC;
        int baseIndex = 100;
        int targetIndex = 100;
        int attackRate = 0;
        int decayRate = 0;
        int attackDuration = 0;
        int decayDuration = 0;
        int decayTransitionTicks = 0;
        double currentCoefficient = 0;
        double attackTargetCoefficient = 0;
        double decayTargetCoefficient = 0;
        double rateFactorPerOutputFrame = 1;
        int64_t elapsedHardwareFramesQ32 = 0;
        int64_t hardwareFramesPerOutputFrameQ32 = kQ32One;
    };
} // namespace mpc::engine
