#pragma once

#include <engine/audio/core/AudioControls.hpp>
#include <engine/audio/core/ChannelFormat.hpp>

#include <string>
#include <memory>

namespace ctoot::audio::mixer {

    class BusControls
            : public ctoot::audio::core::AudioControls
    {

    private:
        std::shared_ptr<ctoot::audio::core::ChannelFormat> channelFormat;

    public:
        virtual std::shared_ptr<ctoot::audio::core::ChannelFormat> getChannelFormat();

    public:
        BusControls(int id, std::string name, std::shared_ptr<ctoot::audio::core::ChannelFormat> format);

    };

}
