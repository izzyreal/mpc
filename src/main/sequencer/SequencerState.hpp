#pragma once

#include "sequencer/SequenceState.hpp"
#include "sequencer/TransportState.hpp"

#include <array>

namespace mpc::sequencer
{
    struct SequencerState
    {
        std::array<SequenceState, Mpc2000XlSpecs::TOTAL_SEQUENCE_COUNT>
            sequences{};
        SequenceIndex selectedSequenceIndex{MinSequenceIndex};
        TransportState transport;
    };
} // namespace mpc::sequencer
