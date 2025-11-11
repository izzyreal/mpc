#pragma once

#include <functional>
#include <cstdint>

namespace mpc::engine
{
    struct EventAfterNFrames
    {
        std::function<void()> f;
        int64_t nFrames = 0;
        int64_t frameCounter = 0;
    };
} // namespace mpc::sequencer
