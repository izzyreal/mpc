#pragma once

#include <engine/audio/mixer/AudioMixerStrip.hpp>

#include <memory>

namespace ctoot::mpc {

    class MpcSampler;

    class MpcSound;

    class MpcBasicSoundPlayerControls;

    class MpcVoice;

    class MpcBasicSoundPlayerChannel final
    {

    private:
        std::shared_ptr<ctoot::mpc::MpcSampler> sampler;
        std::shared_ptr<ctoot::mpc::MpcVoice> voice;
        std::shared_ptr<ctoot::mpc::MpcSound> tempVars;

        std::shared_ptr<ctoot::audio::mixer::AudioMixer> mixer;
        std::shared_ptr<ctoot::audio::fader::FaderControl> fader;

    public:
        void finishVoice();

        void mpcNoteOn(int soundNumber, int velocity, int frameOffset);

        void connectVoice();

        explicit MpcBasicSoundPlayerChannel(std::shared_ptr<MpcBasicSoundPlayerControls> controls);

    };
}
