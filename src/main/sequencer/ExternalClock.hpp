#pragma once

#include <vector>

namespace mpc::sequencer {
    class ExternalClock {
    public:
        void computeTicksForCurrentBuffer(double ppqPosAtStartOfBuffer,
                                          double ppqPositionOfLastBarStart,
                                          int nFrames,
                                          int sampleRate,
                                          double bpm);
        void clearTicks();
        std::vector<int32_t>& getTicksForCurrentBuffer();
        void reset();
        bool areTicksBeingProduced();

    private:
        bool ticksAreBeingProduced = false;
        const double resolution = 96.0;
        std::vector<double> ppqPositions = std::vector<double>(65536, -1);
        std::vector<int32_t> ticks = std::vector<int32_t>(200, -1);
        double previousAbsolutePpqPosition = -1.0;
        double previousRelativePpqPosition = 1.0;
        double previousBpm = 0;
        double previousPpqPositionOfLastBarStart = 0;
    };
}