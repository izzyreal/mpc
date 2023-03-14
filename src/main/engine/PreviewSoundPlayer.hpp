#pragma once

#include "engine/audio/mixer/AudioMixerStrip.hpp"

#include <memory>

namespace mpc::engine {

    class MpcSampler;

    class MpcSound;

    class Voice;

    class PreviewSoundPlayer final
    {

    private:
        std::shared_ptr<mpc::engine::MpcSampler> sampler;
        std::shared_ptr<mpc::engine::Voice> voice;
        std::shared_ptr<mpc::engine::MpcSound> tempVars;

        std::shared_ptr<mpc::engine::audio::mixer::AudioMixer> mixer;
        std::shared_ptr<mpc::engine::FaderControl> fader;

    public:
        void finishVoice();

        void mpcNoteOn(int soundNumber, int velocity, int frameOffset);

        void connectVoice();

        explicit PreviewSoundPlayer(std::shared_ptr<MpcSampler> sampler,
                                    std::shared_ptr<mpc::engine::audio::mixer::AudioMixer> mixer,
                                    std::shared_ptr<Voice> voice);

    };
}
