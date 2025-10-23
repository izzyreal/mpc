#pragma once

#include <memory>
#include <vector>

namespace mpc::engine
{
    class Voice;
    class Drum;

    struct DrumNoteOffContext
    {
        uint64_t noteEventId;
        Drum *drum;
        std::vector<std::shared_ptr<Voice>>* voices{};
        int note{};
        int noteOnStartTick{};
    };
}
