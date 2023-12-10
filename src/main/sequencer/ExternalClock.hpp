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
        std::vector<int>& getTicksForCurrentBuffer();

    private:
        std::vector<int> ticks = std::vector<int>(50, -1);
    };
}