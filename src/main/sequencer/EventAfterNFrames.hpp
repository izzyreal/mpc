#pragma once

#include <functional>
#include <atomic>

namespace mpc::sequencer {

    struct EventAfterNFrames {
        std::atomic<bool> occupied = ATOMIC_VAR_INIT(false);
        std::function<void()> f = []() {};
        unsigned long nFrames = 0;
        unsigned long frameCounter = -1;

        void init(const unsigned long &newNFrames, std::function<void()> callback) {
            occupied.store(true);
            nFrames = newNFrames;
            f = std::move(callback);
        }

        void reset() {
            f = []() {};
            nFrames = 0;
            frameCounter = -1;
            occupied.store(false);
        }
    };
}
