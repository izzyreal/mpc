#pragma once

#include <functional>
#include <atomic>

namespace mpc::sequencer {

    struct EventAfterNFrames {
        std::atomic<bool> occupied = ATOMIC_VAR_INIT(false);
        std::function<void(unsigned int)> f = [](unsigned int frameIndex) {};
        unsigned long nFrames = 0;
        unsigned long frameCounter = -1;

        void init(const unsigned long &newNFrames, std::function<void(unsigned int)> callback) {
            occupied.store(true);
            nFrames = newNFrames;
            f = std::move(callback);
        }

        void reset() {
            f = [](unsigned int frameIndex) {};
            nFrames = 0;
            frameCounter = -1;
            occupied.store(false);
        }
    };
}