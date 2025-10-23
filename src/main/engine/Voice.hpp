#pragma once

#include <atomic>

#include "engine/audio/core/AudioProcess.hpp"
#include "MuteInfo.hpp"
#include "VoiceUtil.hpp"
#include "sampler/VoiceOverlapMode.hpp"

namespace mpc::sampler {
    class Sound;
    class NoteParameters;
}

namespace mpc::engine::control {
    class LawControl;
}

namespace mpc::engine::filter {
    class StateVariableFilter;

    class StateVariableFilterControls;
}

namespace mpc::engine {
    class EnvelopeGenerator;

    class EnvelopeControls;

    class MuteInfo;

    class VoiceState {
        public:
            float sampleRate = 44100.0;
            float inverseNyquist = VoiceUtil::getInverseNyquist(sampleRate);

            // Voice overlap mode when the voice was triggered
            sampler::VoiceOverlapMode voiceOverlapMode;

            // Pointer to currently playing note parameters
            mpc::sampler::NoteParameters *noteParameters = nullptr;

            // Pointer to sample data when the voice was triggered
            std::shared_ptr<const std::vector<float>> sampleData;
            int startTick = -1;
            int tune = 0;
            double increment = 0;
            double position = 0;
            float initialFilterValue = 0;
            bool staticDecay = 0;
            int note = -1;
            int velocity = 0;
            float amplitude = 0;
            int start = 0;
            int end = 0;
            int loopTo = 0;
            int lastFrameIndex = 0;
            bool loopEnabled = false;
            bool finished = true;
            bool isMono = false;
            mpc::engine::MuteInfo muteInfo;
            int frameOffset = 0;
            int decayCounter = 0;
            bool enableEnvs = false;
            int varType = 0;
            int varValue = 0;
            int veloToStart = 0;
            int attackValue = 0;
            int decayValue = 0;
            int veloToAttack = 0;
            int decayMode = 0;
            int veloToLevel = 0;
            float attackMs = 0;
            int finalDecayValue = 0;
            float decayMs = 0;
            float veloToLevelFactor = 0;
            int filtParam = 0;
            float envAmplitude = 0;
            float staticEnvAmp = 0;
            uint64_t noteEventId = 0;
    };

    class Voice : public mpc::engine::audio::core::AudioProcess
    {

    private:
        VoiceState* getActiveState()
        {
            return active.load(std::memory_order_acquire);
        }

        const VoiceState* getActiveState() const {
            return active.load(std::memory_order_acquire);
        }
        
        VoiceState* getInactiveState()
        {
            VoiceState* current = active.load(std::memory_order_acquire);
            return (current == &stateA) ? &stateB : &stateA;
        }

        void swapStates()
        {
            VoiceState* current = active.load(std::memory_order_relaxed);
            VoiceState* other   = (current == &stateA) ? &stateB : &stateA;
            active.store(other, std::memory_order_release);
        }
        
        const int stripNumber = -1;
        const bool basic = false;

        std::vector<float> frame;

        VoiceState stateA;
        VoiceState stateB;
        std::atomic<VoiceState*> active { &stateA };

        mpc::engine::EnvelopeGenerator *staticEnv = nullptr;
        mpc::engine::EnvelopeGenerator *ampEnv = nullptr;
        mpc::engine::EnvelopeGenerator *filterEnv = nullptr;
        mpc::engine::filter::StateVariableFilter *svfLeft = nullptr;
        mpc::engine::filter::StateVariableFilter *svfRight = nullptr;
        mpc::engine::control::LawControl *attack = nullptr;
        mpc::engine::control::LawControl *hold = nullptr;
        mpc::engine::control::LawControl *decay = nullptr;
        mpc::engine::control::LawControl *fattack = nullptr;
        mpc::engine::control::LawControl *fhold = nullptr;
        mpc::engine::control::LawControl *fdecay = nullptr;
        mpc::engine::control::LawControl *shold = nullptr;
        mpc::engine::control::LawControl *reso = nullptr;
        mpc::engine::EnvelopeControls *ampEnvControls = nullptr;
        mpc::engine::EnvelopeControls *staticEnvControls = nullptr;
        mpc::engine::EnvelopeControls *filterEnvControls = nullptr;
        mpc::engine::filter::StateVariableFilterControls *svfControls = nullptr;

        // The master level that is set in the Mixer Setup screen.
        // -Inf, -72, -66, -60, -54, -48, -42, -36, -30, -24, -18, -12, -6, 0, 6 or 12 dB.
        std::atomic_int8_t masterLevel{0};

        void readFrame();
        void initializeSamplerateDependents();
        const std::vector<float> &getFrame();

    public:
        // Called from audio thread
        int processAudio(mpc::engine::audio::core::AudioBuffer *buffer, int nFrames) override;

        // Called from main thread
        void init(int velocity,
                  std::shared_ptr<mpc::sampler::Sound> sound,
                  int note,
                  mpc::sampler::NoteParameters *np,
                  int varType,
                  int varValue,
                  int drumIndex,
                  int frameOffset,
                  bool enableEnvs,
                  int startTick,
                  float engineSampleRate,
                  uint64_t noteEventId);

        uint64_t getNoteEventId();
        
        void startDecay();

        void startDecay(const int offset);

        void finish();

        void setMasterLevel(const int8_t masterLevel);

        int getNote() const;

        bool isFinished() const;

        const mpc::sampler::NoteParameters* getNoteParameters() const;

        sampler::VoiceOverlapMode getVoiceOverlapMode() const;

        int getStripNumber() const;

        int getStartTick() const;

        bool isDecaying() const;

        const MuteInfo &getMuteInfo() const;

    public:
        Voice(const int stripNumber, const bool basic);

        ~Voice();
    };

}
