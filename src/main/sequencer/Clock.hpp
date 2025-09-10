#pragma once

#if __linux__
#include <cstdint>
#endif

#include "FixedVector.hpp"
#include "Sequencer.hpp"

namespace mpc::sequencer {
    class Clock {
    public:
        Clock();
        void computeTicksForCurrentBuffer(const double hostPositionAtStartOfBufferQuarterNotes,
                                          const int nFrames,
                                          const int sampleRate,
                                          const double bpm,
                                          const int64_t timeInSamples);
        void clearTicks();
        const FixedVector<uint16_t, 200>& getTicksForCurrentBuffer();
        void reset();
        bool areTicksBeingProduced();
        
        const double getLastProcessedHostPositionQuarterNotes();

        bool didJumpOccurInLastBuffer();
        void resetJumpOccurredInLastBuffer();
        
        void generateTransportInfo(const float tempo,
                                          const uint32_t sampleRate,
                                          const uint16_t numSamples,
                                          const double playStartPositionQuarterNotes);

    private:
        const double subDiv = 1.0 / Sequencer::TICKS_PER_QUARTER_NOTE;

        FixedVector<double, 65536> positionsInQuarterNotes;
        FixedVector<uint16_t, 200> ticks;

        bool ticksAreBeingProduced;
        double previousHostPositionAtStartOfBufferQuarterNotes;
        double previousAbsolutePositionQuarterNotes;
        double previousRelativePositionQuarterNotes;
        double previousBpm;
        uint32_t previousSampleRate;
        int64_t previousTimeInSamples;
        uint16_t previousBufferSize;
        bool jumpOccurredInLastBuffer;
    };
}
