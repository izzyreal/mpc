#pragma once

#include "IntTypes.hpp"

namespace mpc::sequencer
{
    struct SequencerState
    {
        SequenceIndex selectedSequenceIndex{0};

        struct TransportState
        {
            double positionQuarterNotes = 0.0;
            double playStartPositionQuarterNotes = 0.0;
        } transportState;
    };
} // namespace mpc::sequencer
