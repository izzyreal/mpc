#pragma once
#include "IntTypes.hpp"

#include <cassert>

namespace mpc::sequencer
{
    struct PlaybackTransition
    {
        Tick fromTick{NoTick};
        Tick toTick{NoTick};
        TimeInSamples timeInSamples{NoTimeInSamples};

        bool isActive() const
        {
            return fromTick != NoTick && toTick != NoTick &&
                   timeInSamples != NoTimeInSamples;
        }

        bool isInactive() const
        {
            return fromTick == NoTick && toTick == NoTick &&
                   timeInSamples == NoTimeInSamples;
        }

        void activate(const Tick fromTickToUse, const Tick toTickToUse,
                      const TimeInSamples timeInSamplesToUse)
        {
            assert(fromTickToUse != NoTick && toTickToUse != NoTick &&
                   timeInSamplesToUse != NoTimeInSamples);
            fromTick = fromTickToUse;
            toTick = toTickToUse;
            timeInSamples = timeInSamplesToUse;
        }

        void deactivate()
        {
            fromTick = NoTick;
            toTick = NoTick;
            timeInSamples = NoTimeInSamples;
        }

        void printInfo() const
        {
            if (isInactive())
            {
                printf("Transition inactive\n");
                return;
            }
            printf(
                "Transition fromTick: %lld, toTick: %lld, timeInSamples: %lld\n",
                fromTick, toTick, timeInSamples);
        }
    };
} // namespace mpc::sequencer
