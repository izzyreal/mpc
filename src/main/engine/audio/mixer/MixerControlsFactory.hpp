#pragma once

#include <engine/audio/mixer/MixerControls.hpp>

namespace ctoot::audio::mixer {

    class MixerControlsFactory
    {
    public:
        static void createBusStrips(std::shared_ptr<MixerControls> mixerControls, std::string mainStripName,
                                    std::shared_ptr<ctoot::audio::core::ChannelFormat> mainFormat, int nreturns);

        static void createChannelStrips(std::shared_ptr<MixerControls> mixerControls, int nchannels);

    public:
        static void
        addMixControls(MixerControls *mixerControls, std::shared_ptr<ctoot::audio::core::AudioControlsChain> controls,
                       bool hasMixControls);

    };
}
