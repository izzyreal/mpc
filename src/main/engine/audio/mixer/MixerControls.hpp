#pragma once

#include <engine/control/CompoundControl.hpp>
#include <engine/audio/mixer/BusControls.hpp>
#include <engine/audio/core/AudioControlsChain.hpp>
#include "engine/FaderControl.hpp"

#include <memory>
#include <utility>
#include <vector>
#include <string>
#include "engine/FaderControl.hpp"

namespace mpc::engine::audio::mixer {

    class MixerControls
            : public mpc::engine::control::CompoundControl
    {

    private:
        bool canAddBusses{true};
        std::shared_ptr<BusControls> mainBusControls{nullptr};
        std::vector<std::shared_ptr<BusControls>> auxBusControls{};
        float smoothingFactor{0.05f};

    public:
        float getSmoothingFactor();

        void createAuxBusControls(std::string name, std::shared_ptr<mpc::engine::audio::core::ChannelFormat> format);

        std::shared_ptr<BusControls> getBusControls(std::string name);

        std::shared_ptr<BusControls> getMainBusControls();

        std::vector<std::shared_ptr<BusControls>> getAuxBusControls();

        std::shared_ptr<mpc::engine::audio::core::AudioControlsChain>
        createStripControls(int id, std::string name);

        std::shared_ptr<mpc::engine::audio::core::AudioControlsChain>
        createStripControls(int id, std::string name, bool hasMixControls);

        void addStripControls(std::shared_ptr<CompoundControl> cc);

        std::shared_ptr<mpc::engine::audio::core::AudioControlsChain> getStripControls(std::string name);

        mpc::engine::FaderControl *createFaderControl();

    public:
        MixerControls(std::string name, std::string mainBusName,
                      std::shared_ptr<mpc::engine::audio::core::ChannelFormat> channelFormat);

        MixerControls(std::string name)
                : MixerControls(std::move(name), "Main", mpc::engine::audio::core::ChannelFormat::STEREO())
        {}

        MixerControls(std::string name, float _smoothingFactor)
                : MixerControls(std::move(name))
        { smoothingFactor = _smoothingFactor; };

    };

}
