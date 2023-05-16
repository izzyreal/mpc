#pragma once

#include <vector>
#include <chrono>

namespace mpc::audiomidi {
    class MidiClockInput {
    public:
        void handleTimingMessage(double bufOffsetMs);

    private:
        unsigned int DELTA_COUNT = 30;
        std::vector<double> deltas = std::vector<double>(DELTA_COUNT);
        unsigned int deltaPointer = 0;
        std::chrono::time_point<std::chrono::high_resolution_clock, std::chrono::nanoseconds> previousNow;
        std::chrono::time_point<std::chrono::high_resolution_clock, std::chrono::nanoseconds> zero;
    };
}