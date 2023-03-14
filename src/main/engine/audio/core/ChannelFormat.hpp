#pragma once

#include <engine/audio/core/AudioBuffer.hpp>

#include <string>
#include <cstdint>
#include <vector>
#include <memory>

namespace mpc::engine::audio::core {

    class ChannelFormat
    {

    public:
        virtual int getCount() = 0;

        virtual std::vector<int8_t> getLeft() = 0;

        virtual std::vector<int8_t> getRight() = 0;

        int mix(mpc::engine::audio::core::AudioBuffer *destBuffer, mpc::engine::audio::core::AudioBuffer *sourceBuffer,
                std::vector<float> &gain);

        static std::shared_ptr<ChannelFormat> STEREO();

    };

}
