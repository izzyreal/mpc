#pragma once
#include "IntTypes.hpp"

#include <cassert>

namespace mpc::sequencer
{
    struct PlaybackTransition
    {
        Tick fromTick{NoTick};
        Tick toTick{NoTick};

        bool isActive() const
        {
            return fromTick != NoTick && toTick != NoTick;
        }

        bool isInactive() const
        {
            return fromTick == NoTick && toTick == NoTick;
        }

        void activate(const Tick fromTickToUse, const Tick toTickToUse)
        {
            assert(fromTickToUse != NoTick && toTickToUse != NoTick);
            fromTick = fromTickToUse;
            toTick = toTickToUse;
        }

        void deactivate()
        {
            fromTick = NoTick;
            toTick = NoTick;
        }

        void printInfo() const
        {
            if (isInactive())
            {
                printf("Transition inactive\n");
                return;
            }
            printf("Transition fromTick: %lld, toTick: %lld\n", fromTick, toTick);
        }
    };
} // namespace mpc::sequencer
