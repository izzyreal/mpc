#pragma once

#include <vector>
#include <chrono>

namespace mpc { class Mpc; }

namespace mpc::audiomidi {
    class MidiClockInput {
    public:
        MidiClockInput(mpc::Mpc&);
        void handleTimingMessage(double bufOffsetMs);
        void handleStartMessage();

    private:
        mpc::Mpc& mpc;
        unsigned int DELTA_COUNT = 100;
        std::vector<double> deltas = std::vector<double>(DELTA_COUNT);
        unsigned int deltaPointer = 0;
        bool startIsArmed = false;
        std::chrono::time_point<std::chrono::high_resolution_clock, std::chrono::nanoseconds> previousNow;
        const std::chrono::time_point<std::chrono::high_resolution_clock, std::chrono::nanoseconds> zero;
    };
}