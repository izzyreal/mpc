#pragma once

#include "EventState.hpp"
#include "IntTypes.hpp"
#include "FloatTypes.hpp"
#include "PlaybackTransition.hpp"

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
        SampleRate sampleRate;
        std::vector<RenderedEventState> events;
        PlaybackTransition transition;

        TimeInSamples originSampleTime;
        PositionQuarterNotes originQuarterNotes;
        Tick originTicks;

        TimeInSamples strictValidFrom;
        TimeInSamples strictValidUntil;

        TimeInSamples safeValidFrom;
        Tick safeValidFromTick;
        PositionQuarterNotes safeValidFromQuarterNote;

        TimeInSamples safeValidUntil;
        Tick safeValidUntilTick;
        PositionQuarterNotes safeValidUntilQuarterNote;

        PlaybackState()
        {
            initializeDefaults();
        }

        bool containsTimeInSamplesStrict(const TimeInSamples &t) const
        {
            return t >= strictValidFrom &&
                   t < strictValidUntil;
        }

        Tick getCurrentTick(const Sequence *, TimeInSamples now) const;

        void initializeDefaults()
        {
            sampleRate = NoSampleRate;
            events = {};
            transition.deactivate();

            originSampleTime = NoTimeInSamples;
            originQuarterNotes = NoPositionQuarterNotes;
            originTicks = NoTick;

            strictValidFrom = NoTimeInSamples;
            strictValidUntil = NoTimeInSamples;

            safeValidFrom = NoTimeInSamples;
            safeValidFromTick = NoTick;
            safeValidFromQuarterNote = NoPositionQuarterNotes;

            safeValidUntil = NoTimeInSamples;
            safeValidUntilTick = NoTick;
            safeValidUntilQuarterNote = NoPositionQuarterNotes;
        }

        void printOrigin() const
        {
            printf("== PlaybackState origin ==\n");
            printf("originSampleTime: %lld\n", originSampleTime);
            printf("originTicks: %lld\n", originTicks);
            printf("originQuarterNotes: %f\n", originQuarterNotes);
        }
    };
} // namespace mpc::sequencer