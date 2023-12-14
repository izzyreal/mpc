#pragma once

#include <vector>

namespace mpc::sequencer {
    class ExternalClock {
    public:
        void computeTicksForCurrentBuffer(double ppqPosAtStartOfBuffer,
                                          double firstBarStartAtStartOfBuffer,
                                          int nFrames,
                                          int sampleRate,
                                          double bpm);
        void clearTicks();
        std::vector<double>& getTicksForCurrentBuffer();
        void reset();

    private:
        const double resolution = 96.0;
        std::vector<double> ticks = std::vector<double>(200, -1);
        unsigned long createdTicksSincePlayStart = 0;
        double firstBarStart = -1;
    };
}