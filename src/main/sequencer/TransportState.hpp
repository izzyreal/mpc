#pragma once

#include "FloatTypes.hpp"

namespace mpc::sequencer
{
    struct TransportState
    {
        PositionQuarterNotes positionQuarterNotes = NoPositionQuarterNotes;
        bool sequencerRunning = false;
        bool recording = false;
        bool overdubbing = false;
        bool countEnabled = true;
    };
} // namespace mpc::sequencer
