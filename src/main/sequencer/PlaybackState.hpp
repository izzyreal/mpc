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
        PositionQuarterNotes playOffsetQuarterNotes;
        TimeInSamples currentTimeInSamples{NoTimeInSamples};
        TimeInSamples strictValidUntilTimeInSamples{NoTimeInSamples};

        TimeInSamples safeValidFromTimeInSamples{NoTimeInSamples};
        Tick safeValidFromTick{NoTick};
        PositionQuarterNotes safeValidFromQuarterNote{NoPositionQuarterNotes};

        TimeInSamples safeValidUntilTimeInSamples{NoTimeInSamples};
        Tick safeValidUntilTick{NoTick};
        PositionQuarterNotes safeValidUntilQuarterNote{NoPositionQuarterNotes};

        std::vector<RenderedEventState> events{};
    };
} // namespace mpc::sequencer