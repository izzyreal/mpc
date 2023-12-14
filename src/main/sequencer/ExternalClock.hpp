#pragma once

#include <vector>

namespace mpc::sequencer {
    class ExternalClock {
    public:
        void computeTicksForCurrentBuffer(double ppqPosAtStartOfBuffer,
                                          int nFrames,
                                          int sampleRate,
                                          double bpm);
        void clearTicks();
        std::vector<double>& getTicksForCurrentBuffer();
        void reset();

    private:
        const double resolution = 96.0;
        std::vector<double> ticks = std::vector<double>(200, -1);
        unsigned long long createdTicksSincePlayStart = -1;
    };
}