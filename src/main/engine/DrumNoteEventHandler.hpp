#pragma once

#include "DrumNoteOnContext.hpp"
#include "DrumNoteOffContext.hpp"

namespace mpc::engine
{
    class DrumNoteEventHandler final
    {
    public:
        static void noteOn(const DrumNoteOnContext& ctx);
        static void noteOff(const DrumNoteOffContext& ctx);
    };
}
