#pragma once

#include "sequencer/EventState.hpp"

#include <vector>

namespace mpc::sequencer
{
    struct TrackEventState
    {
        std::vector<EventState> events;
        EventIndex eventIndex{0};
    };
} // namespace mpc::sequencer
