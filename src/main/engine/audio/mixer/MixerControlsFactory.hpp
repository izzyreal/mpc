#pragma once

#include "engine/audio/mixer/MixerControls.hpp"

namespace mpc::engine::audio::mixer
{

    class MixerControlsFactory
    {
    public:
        static void
        createBusStrips(const std::shared_ptr<MixerControls> &mixerControls,
                        const std::string &mainStripName);

        static void
        createChannelStrips(const std::shared_ptr<MixerControls> &mixerControls,
                            int nchannels);

    public:
        static void addMixControls(
            MixerControls *mixerControls,
            const std::shared_ptr<mpc::engine::audio::core::AudioControlsChain>
                &controls,
            bool hasMixControls);
    };
} // namespace mpc::engine::audio::mixer
