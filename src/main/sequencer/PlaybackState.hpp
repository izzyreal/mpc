#pragma once

#include "EventState.hpp"
#include "IntTypes.hpp"
#include "FloatTypes.hpp"

#include <vector>

namespace mpc::sequencer
{
    class Sequence;

    struct RenderedEventState
    {
        EventState eventState;
        TimeInSamples timeInSamples;
    };

    struct PlaybackState
    {
        SampleRate sampleRate{NoSampleRate};

        TimeInSamples originSampleTime{NoTimeInSamples};
        PositionQuarterNotes originQuarterNotes{NoPositionQuarterNotes};
        Tick originTicks{NoTick};

        TimeInSamples strictValidFromTimeInSamples{NoTimeInSamples};
        TimeInSamples strictValidUntilTimeInSamples{NoTimeInSamples};

        TimeInSamples safeValidFromTimeInSamples{NoTimeInSamples};
        Tick safeValidFromTick{NoTick};
        PositionQuarterNotes safeValidFromQuarterNote{NoPositionQuarterNotes};

        TimeInSamples safeValidUntilTimeInSamples{NoTimeInSamples};
        Tick safeValidUntilTick{NoTick};
        PositionQuarterNotes safeValidUntilQuarterNote{NoPositionQuarterNotes};

        std::vector<RenderedEventState> events{};

        bool containsTimeInSamplesStrict(const TimeInSamples &t) const
        {
            return t >= strictValidFromTimeInSamples &&
                   t < strictValidUntilTimeInSamples;
        }

        Tick getCurrentTick(const Sequence *, TimeInSamples now) const;
    };
} // namespace mpc::sequencer