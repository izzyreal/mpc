#pragma once

#include <engine/audio/mixer/MixerControls.hpp>

namespace mpc::engine::audio::mixer
{

    class MixerControlsFactory
    {
    public:
        static void createBusStrips(std::shared_ptr<MixerControls> mixerControls, std::string mainStripName);

        static void createChannelStrips(std::shared_ptr<MixerControls> mixerControls, int nchannels);

    public:
        static void
        addMixControls(MixerControls *mixerControls, std::shared_ptr<mpc::engine::audio::core::AudioControlsChain> controls,
                       bool hasMixControls);
    };
} // namespace mpc::engine::audio::mixer
