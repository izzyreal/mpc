#pragma once

#include "sequencer/SequenceState.hpp"
#include "sequencer/TransportState.hpp"
#include "sequencer/SongState.hpp"

#include <array>

namespace mpc::sequencer
{
    struct SequencerState
    {
        std::array<SequenceState, Mpc2000XlSpecs::TOTAL_SEQUENCE_COUNT>
            sequences{};
        std::array<SongState, Mpc2000XlSpecs::MAX_SONG_COUNT> songs{};
        SequenceIndex selectedSequenceIndex{MinSequenceIndex};
        SongStepIndex selectedSongStepIndex{MinSongStepIndex};
        SongIndex selectedSongIndex{MinSongIndex};
        TransportState transport;
    };
} // namespace mpc::sequencer
