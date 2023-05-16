#pragma once

#include <vector>
#include <chrono>

namespace mpc::audiomidi {
    class MidiClockInput {
    public:
        void handleTimingMessage();

    private:
        unsigned int DELTA_COUNT = 5;
        std::vector<unsigned int> deltas = std::vector<unsigned int>(DELTA_COUNT);
        unsigned char deltaPointer = 0;
        std::chrono::time_point<std::chrono::high_resolution_clock > previousNow;
    };
}