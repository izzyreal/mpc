#pragma once

#include "EventState.hpp"
#include "IntTypes.hpp"
#include "FloatTypes.hpp"

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
        bool countEnabled;

        SampleRate sampleRate{NoSampleRate};
        PositionQuarterNotes playOffset;
        TimeInSamples currentTime{NoTimeInSamples};
        TimeInSamples validUntil{NoTimeInSamples};
        std::vector<RenderedEventState> events{};
    };
} // namespace mpc::sequencer