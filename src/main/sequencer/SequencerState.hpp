#pragma once

namespace mpc::sequencer
{
    struct SequencerState
    {
        struct TransportState
        {
            double positionQuarterNotes = 0.0;
            double playStartPositionQuarterNotes = 0.0;
        } transportState;
    };
} // namespace mpc::sequencer
