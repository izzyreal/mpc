#pragma once

#if __linux__
#include <cstdint>
#endif

#include "FixedVector.hpp"

namespace mpc::sequencer {
    class ExternalClock {
    public:
        ExternalClock();
        void computeTicksForCurrentBuffer(const double ppqPosAtStartOfBuffer,
                                          const double ppqPositionOfLastBarStart,
                                          const int nFrames,
                                          const int sampleRate,
                                          const double bpm,
                                          const int64_t timeInSamples);
        void clearTicks();
        const FixedVector<uint16_t, 200>& getTicksForCurrentBuffer();
        void reset();
        bool areTicksBeingProduced();
        
        const double getLastProcessedIncomingPpqPosition();

        void setPreviousAbsolutePpqPosition(const double ppqPosition);

        bool didJumpOccurInLastBuffer();
        void resetJumpOccurredInLastBuffer();

    private:
        const double resolution = 96.0;
        const double subDiv = 1.0 / resolution;

        FixedVector<double, 65536> ppqPositions;
        FixedVector<uint16_t, 200> ticks;

        bool ticksAreBeingProduced;
        double previousIncomingPpqPosition;
        double previousAbsolutePpqPosition;
        double previousRelativePpqPosition;
        double previousBpm;
        double previousPpqPositionOfLastBarStart;
        uint32_t previousSampleRate;
        int64_t previousTimeInSamples;
        uint16_t previousBufferSize;
        bool jumpOccurredInLastBuffer;
    };
}
