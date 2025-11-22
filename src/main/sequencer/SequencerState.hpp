#pragma once

#include "IntTypes.hpp"
#include "sequencer/TimeSignature.hpp"

#include <array>

namespace mpc::sequencer
{
    struct TransportState
    {
        double positionQuarterNotes = 0.0;
        double playStartPositionQuarterNotes = 0.0;
    };

    struct SequenceState
    {
        std::array<TimeSignature, Mpc2000XlSpecs::MAX_BAR_COUNT> timeSignatures;
        std::array<Tick, Mpc2000XlSpecs::MAX_BAR_COUNT> barLengths;
    };

    struct SequencerState
    {
        SequenceIndex selectedSequenceIndex{MinSequenceIndex};
        TransportState transportState;
        std::array<SequenceState, Mpc2000XlSpecs::SEQUENCE_COUNT> sequences;
    };

} // namespace mpc::sequencer
