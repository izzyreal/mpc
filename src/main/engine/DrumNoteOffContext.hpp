#pragma once

#include <memory>
#include <vector>
#include <map>

namespace mpc::engine
{
    class Voice;
    class Drum;

    struct DrumNoteOffContext
    {
        uint64_t noteEventId;
        int drumIndex;
        std::map<int, int> *drumSimultA;
        std::map<int, int> *drumSimultB;
        std::vector<std::shared_ptr<Voice>> *voices{};
        int note{};
        int noteOnStartTick{};
    };
} // namespace mpc::engine
