#pragma once

#include "FloatTypes.hpp"
#include "IntTypes.hpp"

namespace mpc::sequencer
{
    struct TransportState
    {
        PositionQuarterNotes positionQuarterNotes = NoPositionQuarterNotes;
        PositionQuarterNotes playStartPositionQuarterNotes = NoPositionQuarterNotes;
        Tick metronomeOnlyPositionTicks = 0;
        bool sequencerRunning = false;
        bool recordingEnabled = false;
        bool overdubbingEnabled = false;
        bool countEnabled = true;
        bool countingIn = false;
        Tick countInStartPos = NoTick;
        Tick countInEndPos = NoTick;
        bool metronomeOnlyEnabled = false;
        bool endOfSong = false;
        int playedSongStepRepetitionCount = 0;
    };
} // namespace mpc::sequencer
