#pragma once
#include "IntTypes.hpp"

namespace mpc::sequencer
{
    struct PlaybackTransition
    {
        Tick fromTick{NoTick};
        Tick toTick{NoTick};

        bool isActive() const
        {
            return fromTick == NoTick && toTick == NoTick;
        }

        void activate(const Tick fromTickToUse, const Tick toTickToUse)
        {
            fromTick = fromTickToUse;
            toTick = toTickToUse;
        }

        void deactivate()
        {
            fromTick = NoTick;
            toTick = NoTick;
        }
    };
} // namespace mpc::sequencer
