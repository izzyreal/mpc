#pragma once

#include <engine/audio/mixer/AudioMixerStrip.hpp>

#include <memory>

namespace ctoot::mpc {

    class MpcSampler;

    class MpcSound;

    class MpcVoice;

    class PreviewSoundPlayer final
    {

    private:
        std::shared_ptr<ctoot::mpc::MpcSampler> sampler;
        std::shared_ptr<ctoot::mpc::MpcVoice> voice;
        std::shared_ptr<ctoot::mpc::MpcSound> tempVars;

        std::shared_ptr<ctoot::audio::mixer::AudioMixer> mixer;
        std::shared_ptr<ctoot::mpc::MpcFaderControl> fader;

    public:
        void finishVoice();

        void mpcNoteOn(int soundNumber, int velocity, int frameOffset);

        void connectVoice();

        explicit PreviewSoundPlayer(std::shared_ptr<MpcSampler> sampler,
                                    std::shared_ptr<ctoot::audio::mixer::AudioMixer> mixer,
                                    std::shared_ptr<MpcVoice> voice);

    };
}
