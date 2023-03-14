#pragma once

#include <engine/audio/core/ChannelFormat.hpp>
#include <cstdint>

namespace mpc::engine::audio::core {

    class ChannelFormat2 final
            : public ChannelFormat
    {

    private:
        std::vector<int8_t> left{0};
        std::vector<int8_t> right{1};

    public:
        int getCount() override;

        std::vector<int8_t> getLeft() override;

        std::vector<int8_t> getRight() override;

        ChannelFormat2();

    };
}
