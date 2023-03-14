#pragma once

#include <engine/control/CompoundControl.hpp>
#include <engine/audio/mixer/BusControls.hpp>
#include <engine/audio/core/AudioControlsChain.hpp>
#include <engine/mpc/FaderControl.hpp>

#include <memory>
#include <utility>
#include <vector>
#include <string>
#include "engine/mpc/FaderControl.hpp"

namespace ctoot::audio::mixer {

    class MixerControls
            : public ctoot::control::CompoundControl
    {

    private:
        bool canAddBusses{true};
        std::shared_ptr<BusControls> mainBusControls{nullptr};
        std::vector<std::shared_ptr<BusControls>> auxBusControls{};
        float smoothingFactor{0.05f};

    public:
        float getSmoothingFactor();

        void createAuxBusControls(std::string name, std::shared_ptr<ctoot::audio::core::ChannelFormat> format);

        std::shared_ptr<BusControls> getBusControls(std::string name);

        std::shared_ptr<BusControls> getMainBusControls();

        std::vector<std::shared_ptr<BusControls>> getAuxBusControls();

        std::shared_ptr<ctoot::audio::core::AudioControlsChain>
        createStripControls(int id, std::string name);

        std::shared_ptr<ctoot::audio::core::AudioControlsChain>
        createStripControls(int id, std::string name, bool hasMixControls);

        void addStripControls(std::shared_ptr<CompoundControl> cc);

        std::shared_ptr<ctoot::audio::core::AudioControlsChain> getStripControls(std::string name);

        ctoot::mpc::FaderControl *createFaderControl();

    public:
        MixerControls(std::string name, std::string mainBusName,
                      std::shared_ptr<ctoot::audio::core::ChannelFormat> channelFormat);

        MixerControls(std::string name)
                : MixerControls(std::move(name), "Main", ctoot::audio::core::ChannelFormat::STEREO())
        {}

        MixerControls(std::string name, float _smoothingFactor)
                : MixerControls(std::move(name))
        { smoothingFactor = _smoothingFactor; };

    };

}
