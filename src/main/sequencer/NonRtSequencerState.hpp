#pragma once

#include "sequencer/EventState.hpp"

#include <array>
#include <vector>

namespace mpc::sequencer
{
    struct NonRtTrackState
    {
        std::vector<EventState> events;
    };

    struct NonRtSequenceState
    {
        std::array<NonRtTrackState, Mpc2000XlSpecs::TOTAL_TRACK_COUNT> tracks{};
    };

    struct NonRtSequencerState
    {
        std::array<NonRtSequenceState, Mpc2000XlSpecs::TOTAL_SEQUENCE_COUNT> sequences{};
    };
} // namespace mpc::sequencer
