#pragma once

#include <cstdint>
#include <variant>

namespace mpc::sequencer
{
    struct SetPositionQuarterNotes
    {
        double positionQuarterNotes;
    };

    struct BumpPositionByTicks
    {
        uint8_t ticks;
    };

    struct SetSongModeEnabled
    {
        bool songModeEnabled;
    };

    using SequencerMessage = std::variant<
        SetPositionQuarterNotes,
        BumpPositionByTicks,
        SetSongModeEnabled
    >;
}

