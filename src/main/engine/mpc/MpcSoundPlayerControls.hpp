#pragma once

#include <engine/audio/server/AudioServer.hpp>
#include <engine/audio/mixer/AudioMixer.hpp>

#include <memory>

namespace ctoot::mpc {
    class MpcMixerSetupGui;

    class MpcSampler;

    class MpcVoice;

    class MpcSoundPlayerControls
            : public ctoot::control::CompoundControl
    {

    private:
        static std::string NAME_;
        std::shared_ptr<ctoot::mpc::MpcSampler> sampler;
        std::shared_ptr<ctoot::audio::mixer::AudioMixer> mixer;
        int drumNumber{0};
        std::shared_ptr<ctoot::audio::server::AudioServer> server;
        ctoot::mpc::MpcMixerSetupGui *mixerSetupGui;
        std::vector<std::shared_ptr<MpcVoice>> voices;

    public:
        std::vector<std::shared_ptr<MpcVoice>> getVoices();

        std::shared_ptr<ctoot::mpc::MpcSampler> getSampler();

        int getDrumIndex();

        std::shared_ptr<ctoot::audio::mixer::AudioMixer> getMixer();

        ctoot::audio::server::AudioServer *getServer();

        ctoot::mpc::MpcMixerSetupGui *getMixerSetupGui();

    public:
        MpcSoundPlayerControls(
                std::shared_ptr<MpcSampler> sampler,
                int drumNumber,
                std::shared_ptr<ctoot::audio::mixer::AudioMixer> mixer,
                std::shared_ptr<ctoot::audio::server::AudioServer> server,
                MpcMixerSetupGui *mixerSetupGui,
                std::vector<std::shared_ptr<MpcVoice>> voices);

    };
}
