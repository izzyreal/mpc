#pragma once

#include "EventState.hpp"
#include "IntTypes.hpp"

#include <vector>

namespace mpc::sequencer
{
    struct RenderedEventState
    {
        EventState eventState;
        TimeInSamples timeInSamples;
    };

    struct PlaybackState
    {
        TimeInSamples timeInSamples;
        std::vector<RenderedEventState> events;
    };
} // namespace mpc::sequencer