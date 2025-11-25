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
        SampleRate sampleRate{NoSampleRate};
        TimeInSamples currentTime{NoTimeInSamples};
        TimeInSamples validUntil{NoTimeInSamples};
        std::vector<RenderedEventState> events{};
    };
} // namespace mpc::sequencer