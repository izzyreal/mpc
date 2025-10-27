#pragma once

#include <functional>
#include <atomic>

namespace mpc::sequencer
{

    struct EventAfterNFrames
    {
        std::atomic<bool> occupied = ATOMIC_VAR_INIT(false);
        std::function<void()> f = []() {};
        int64_t nFrames = 0;
        int64_t frameCounter = -1;

        void init(const unsigned long &newNFrames,
                  std::function<void()> callback)
        {
            occupied.store(true);
            nFrames = newNFrames;
            f = std::move(callback);
        }

        void reset()
        {
            f = []() {};
            nFrames = 0;
            frameCounter = -1;
            occupied.store(false);
        }
    };
} // namespace mpc::sequencer
