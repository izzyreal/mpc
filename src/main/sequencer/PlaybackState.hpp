#pragma once

#include "EventState.hpp"
#include "IntTypes.hpp"
#include "FloatTypes.hpp"
#include "PlaybackTransition.hpp"

#include <vector>

namespace mpc::sequencer
{
    class SequenceStateView;
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

        TimeInSamples lastTransitionTime;
        double lastTransitionTick;

        TimeInSamples strictValidFrom;
        double strictValidFromTick;

        TimeInSamples strictValidUntil;
        double strictValidUntilTick;

        // TimeInSamples safeValidFrom;
        // double safeValidFromTick;
        //
        // TimeInSamples safeValidUntil;
        // double safeValidUntilTick;

        PlaybackState()
        {
            initializeDefaults();
        }

        // PositionQuarterNotes getSafeValidFromQN() const;
        // PositionQuarterNotes getSafeValidUntilQN() const;
        PositionQuarterNotes getLastTransitionQN() const;
        PositionQuarterNotes getStrictValidFromQN() const;
        PositionQuarterNotes getStrictValidUntilQN() const;

        bool containsTimeInSamplesStrict(const TimeInSamples &t) const
        {
            return t >= strictValidFrom &&
                   t < strictValidUntil;
        }

        TimeInSamples strictLengthInSamples() const
        {
            return strictValidUntil - strictValidFrom;
        }

        double strictLengthInTicks() const
        {
            return strictValidUntilTick - strictValidFromTick;
        }

        double getCurrentTick(const SequenceStateView &, TimeInSamples now) const;

        void initializeDefaults()
        {
            sampleRate = NoSampleRate;
            events = {};
            transition.deactivate();

            lastTransitionTime = NoTimeInSamples;
            lastTransitionTick = NoTick;

            strictValidFrom = NoTimeInSamples;
            strictValidFromTick = NoTick;

            strictValidUntil = NoTimeInSamples;
            strictValidUntilTick = NoTick;

            // safeValidFrom = NoTimeInSamples;
            // safeValidFromTick = NoTick;
            //
            // safeValidUntil = NoTimeInSamples;
            // safeValidUntilTick = NoTick;
        }

        void printOrigin() const
        {
            printf("== PlaybackState ==\n");
            printf("lastTransitionTime: %lld\n", lastTransitionTime);
            printf("lastTransitionTick: %f\n", lastTransitionTick);
            printf("lastTransitionQN: %f\n", getLastTransitionQN());
        }
    };
} // namespace mpc::sequencer