#pragma once

#include <engine/audio/mixer/MixProcess.hpp>
#include <engine/audio/mixer/MixVariables.hpp>

namespace mpc::engine::audio::mixer {

    class MainMixProcess
            : public MixProcess
    {
    public:
        std::shared_ptr<AudioMixerStrip> nextRoutedStrip;

    public:
        mpc::engine::control::EnumControl *routeControl{nullptr};

    public:
        AudioMixerStrip *getRoutedStrip() override;

    public:
        MainMixProcess(std::shared_ptr<AudioMixerStrip> strip, std::shared_ptr<MixVariables> vars, AudioMixer *mixer);

    };

}
