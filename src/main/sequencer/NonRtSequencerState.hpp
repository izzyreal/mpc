#pragma once

#include "sequencer/EventState.hpp"

#include <vector>

namespace mpc::sequencer
{
    struct NonRtTrackState
    {
        std::vector<EventState> events;
    };

    struct NonRtSequenceState
    {
        std::vector<NonRtTrackState> tracks;
    };

    struct NonRtSequencerState
    {
        std::vector<NonRtSequenceState> sequences;
    };
} // namespace mpc::sequencer
