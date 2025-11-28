#pragma once

#include "sequencer/SequenceState.hpp"
#include "sequencer/PlaybackState.hpp"
#include "sequencer/TransportState.hpp"

#include <array>
#include <vector>

namespace mpc::sequencer
{
    struct SequencerState
    {
        std::array<SequenceState, Mpc2000XlSpecs::TOTAL_SEQUENCE_COUNT>
            sequences{};
        PlaybackState playbackState{};
        SequenceIndex selectedSequenceIndex{MinSequenceIndex};
        TransportState transport;
    };
} // namespace mpc::sequencer
