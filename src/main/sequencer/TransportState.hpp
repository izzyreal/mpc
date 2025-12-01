#pragma once

#include "FloatTypes.hpp"
#include "IntTypes.hpp"

namespace mpc::sequencer
{
    struct TransportState
    {
        PositionQuarterNotes positionQuarterNotes = NoPositionQuarterNotes;
        PositionQuarterNotes playStartPositionQuarterNotes = NoPositionQuarterNotes;
        Tick metronomeOnlyPositionTicks = NoTick;
        bool sequencerRunning = false;
        bool recordingEnabled = false;
        bool overdubbingEnabled = false;
        bool countEnabled = true;
        bool countingIn = false;
        bool metronomeOnlyEnabled = false;
    };
} // namespace mpc::sequencer
