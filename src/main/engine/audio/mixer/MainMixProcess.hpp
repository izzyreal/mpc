#pragma once

#include <engine/audio/mixer/MixProcess.hpp>

namespace mpc::engine::audio::mixer {

    class MixControls;

    class MainMixProcess : public MixProcess
    {
    public:
        std::shared_ptr<AudioMixerStrip> nextRoutedStrip;

    public:
        mpc::engine::control::EnumControl *routeControl{nullptr};

    public:
        AudioMixerStrip *getRoutedStrip() override;

    public:
        MainMixProcess(const std::shared_ptr<AudioMixerStrip>& strip, const std::shared_ptr<MixControls>& mixControls, AudioMixer *mixer);

    };

}
