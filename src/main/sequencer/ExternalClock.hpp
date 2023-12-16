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
        std::vector<double> ppqPositions = std::vector<double>(2048, -1);
        std::vector<double> ticks = std::vector<double>(200, -1);
        double previousAbsolutePpqPosition = 0.0;
        double previousRelativePpqPosition = 1.0;
    };
}