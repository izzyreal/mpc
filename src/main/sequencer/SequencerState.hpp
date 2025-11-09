#pragma once

namespace mpc::sequencer
{
    struct SequencerState
    {
        double positionQuarterNotes = 0.0;
        double playStartPositionQuarterNotes = 0.0;
        bool songModeEnabled = false;
    };
}
