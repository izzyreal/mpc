#pragma once

#include "IntTypes.hpp"

namespace mpc::sequencer
{
    struct TransportState
    {
        double positionQuarterNotes = 0.0;
        double playStartPositionQuarterNotes = 0.0;
    };

    struct SequencerState
    {
        SequenceIndex selectedSequenceIndex{MinSequenceIndex};
        TransportState transportState;
    };

} // namespace mpc::sequencer
