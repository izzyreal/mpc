#pragma once

#include <vector>
#include <chrono>

namespace mpc::audiomidi {
    class MidiClockInput {
    public:
        MidiClockInput();
        void handleTimingMessage(double bufOffsetMs);
        void handleStartMessage();

    private:
        unsigned int DELTA_COUNT = 100;
        std::vector<double> deltas = std::vector<double>(DELTA_COUNT);
        unsigned int deltaPointer = 0;
        std::chrono::time_point<std::chrono::high_resolution_clock, std::chrono::nanoseconds> previousNow;
        const std::chrono::time_point<std::chrono::high_resolution_clock, std::chrono::nanoseconds> zero;
    };
}