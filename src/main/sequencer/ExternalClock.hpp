#pragma once

#if __linux__
#include <cstdint>
#endif

#include "FixedVector.hpp"

namespace mpc::sequencer {
    class ExternalClock {
    public:
        ExternalClock();
        void computeTicksForCurrentBuffer(double ppqPosAtStartOfBuffer,
                                          double ppqPositionOfLastBarStart,
                                          int nFrames,
                                          int sampleRate,
                                          double bpm);
        void clearTicks();
        const FixedVector<uint16_t, 200>& getTicksForCurrentBuffer();
        void reset();
        bool areTicksBeingProduced();

    private:
        const double resolution = 96.0;
        const double subDiv = 1.0 / resolution;

        FixedVector<double, 65536> ppqPositions;
        FixedVector<uint16_t, 200> ticks;

        bool ticksAreBeingProduced;
        double previousAbsolutePpqPosition;
        double previousRelativePpqPosition;
        double previousBpm;
        double previousPpqPositionOfLastBarStart;
    };
}
