#pragma once

#include "engine/audio/mixer/MixProcess.hpp"

namespace mpc::engine::audio::mixer
{

    class MixControls;

    class MainMixProcess : public MixProcess
    {
    public:
        std::shared_ptr<AudioMixerStrip> nextRoutedStrip;

        control::EnumControl *routeControl{nullptr};

        AudioMixerStrip *getRoutedStrip() override;

        MainMixProcess(const std::shared_ptr<AudioMixerStrip> &strip,
                       const std::shared_ptr<MixControls> &mixControls,
                       AudioMixer *mixer);
    };

} // namespace mpc::engine::audio::mixer
