#pragma once

#include "sequencer/EventState.hpp"

#include <vector>

namespace mpc::sequencer
{
    struct TrackState
    {
        std::vector<EventState> events;
    };
} // namespace mpc::sequencer
