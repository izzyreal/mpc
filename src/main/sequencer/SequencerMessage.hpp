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

    using SequencerMessage = std::variant<
        SetPositionQuarterNotes,
        BumpPositionByTicks
    >;
}

