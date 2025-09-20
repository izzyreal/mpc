#pragma once

#include <atomic>
#include <memory>

#include "engine/audio/core/AudioProcess.hpp"
#include "MuteInfo.hpp"

namespace mpc::sampler {
    class Sound;
    class NoteParameters;
}

namespace mpc::engine::control {
    class LawControl;
    class BooleanControl;
}

namespace mpc::engine::filter {
    class StateVariableFilter;

    class StateVariableFilterControls;
}

namespace mpc::engine {
    class EnvelopeGenerator;

    class EnvelopeControls;

    class MuteInfo;

    class VoiceState;

    class Voice
            : public mpc::engine::audio::core::AudioProcess
    {
    private:
        std::shared_ptr<VoiceState> currentState;

    private:
        static float midiFreq(float pitch);
        static float midiFreqImpl(int pitch);
        float sampleRate = 44100.0;

        constexpr static const float STATIC_ATTACK_LENGTH = 10.92f;
        constexpr static const float STATIC_DECAY_LENGTH = 109.2f;
        static const int MAX_ATTACK_LENGTH_MS = 3000;
        static const int MAX_DECAY_LENGTH_MS = 2600;
        static const int MAX_ATTACK_LENGTH_SAMPLES = 132300;
        static const int MAX_DECAY_LENGTH_SAMPLES = 114660;
        static const int ATTACK_INDEX = 0;
        static const int HOLD_INDEX = 1;
        static const int DECAY_INDEX = 2;
        static const int RESO_INDEX = 1;
        static const int SVF_OFFSET = 48;
        static const int AMPENV_OFFSET = 64;
        static float getInverseNyquist(int sampleRate)
        { return 2.f / sampleRate; }

        static std::vector<float> EMPTY_FRAME;

        std::atomic_int8_t masterLevel{0};

    public:
        void open() override;
        int processAudio(mpc::engine::audio::core::AudioBuffer *buffer, int nFrames) override;
        void close() override;

        bool isFinished();
        void init(int velocity,
                  std::shared_ptr<mpc::sampler::Sound> mpcSound,
                  int note,
                  mpc::sampler::NoteParameters *np,
                  int varType,
                  int varValue,
                  int muteNote,
                  int muteDrum,
                  int frameOffset,
                  bool enableEnvs,
                  int startTick,
                  int newDuration);

        std::vector<float> &getFrame();
        int getNote();
        mpc::sampler::NoteParameters *getNoteParameters();
        void startDecay();
        int getVoiceOverlap();
        int getStripNumber();
        int getStartTick();
        bool isDecaying();
        mpc::engine::MuteInfo &getMuteInfo();
        void startDecay(int offset);
        void finish();
        void setMasterLevel(int8_t masterLevel);
        static std::vector<float> &freqTable();

    public:
        Voice(int stripNumber, bool basic);
        ~Voice() = default;
    };

    class VoiceState {
    public:
        int voiceOverlapMode{};
        mpc::sampler::NoteParameters *noteParameters = nullptr;
        std::shared_ptr<mpc::sampler::Sound> mpcSound;
        int startTick = -1;
        int tune = 0;
        double increment = 0;
        double position = 0;
        std::vector<float> *sampleData = nullptr;
        mpc::engine::EnvelopeGenerator *staticEnv = nullptr;
        mpc::engine::EnvelopeGenerator *ampEnv = nullptr;
        mpc::engine::EnvelopeGenerator *filterEnv = nullptr;
        float initialFilterValue = 0;
        bool staticDecay = 0;
        int note = -1;
        int velocity = 0;
        float amplitude = 0;
        mpc::engine::filter::StateVariableFilter *svfLeft = nullptr;
        mpc::engine::filter::StateVariableFilter *svfRight = nullptr;
        int end = 0;
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
        bool finished = true;
        int stripNumber = -1;

        mpc::engine::MuteInfo muteInfo;
        int frameOffset = 0;
        bool basic = false;
        int decayCounter = 0;
        bool enableEnvs = false;
        std::vector<float> tempFrame;
        int duration = -1;
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
        double frac = 0;
        int k = 0;
        int j = 0;
        std::vector<float> frame;
    };

}

