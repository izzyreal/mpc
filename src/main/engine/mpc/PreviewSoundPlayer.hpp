#pragma once

#include <engine/audio/mixer/AudioMixerStrip.hpp>

#include <memory>

namespace ctoot::mpc {

    class MpcSampler;

    class MpcSound;

    class Voice;

    class PreviewSoundPlayer final
    {

    private:
        std::shared_ptr<ctoot::mpc::MpcSampler> sampler;
        std::shared_ptr<ctoot::mpc::Voice> voice;
        std::shared_ptr<ctoot::mpc::MpcSound> tempVars;

        std::shared_ptr<ctoot::audio::mixer::AudioMixer> mixer;
        std::shared_ptr<ctoot::mpc::FaderControl> fader;

    public:
        void finishVoice();

        void mpcNoteOn(int soundNumber, int velocity, int frameOffset);

        void connectVoice();

        explicit PreviewSoundPlayer(std::shared_ptr<MpcSampler> sampler,
                                    std::shared_ptr<ctoot::audio::mixer::AudioMixer> mixer,
                                    std::shared_ptr<Voice> voice);

    };
}
