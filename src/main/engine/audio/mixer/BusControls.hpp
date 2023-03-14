#pragma once

#include <engine/audio/core/AudioControls.hpp>
#include <engine/audio/core/ChannelFormat.hpp>

#include <string>
#include <memory>

namespace mpc::engine::audio::mixer {

    class BusControls
            : public mpc::engine::audio::core::AudioControls
    {

    private:
        std::shared_ptr<mpc::engine::audio::core::ChannelFormat> channelFormat;

    public:
        virtual std::shared_ptr<mpc::engine::audio::core::ChannelFormat> getChannelFormat();

    public:
        BusControls(int id, std::string name, std::shared_ptr<mpc::engine::audio::core::ChannelFormat> format);

    };

}
