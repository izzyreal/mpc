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
        std::vector<int32_t>& getTicksForCurrentBuffer();

    private:
        std::vector<int32_t> ticks = std::vector<int>(100, -1);
    };
}