#pragma once

#include "sequencer/EventState.hpp"

namespace mpc::sequencer
{
    struct TrackState
    {
        EventState *head;
    };
} // namespace mpc::sequencer
