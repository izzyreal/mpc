#pragma once

#include "IntTypes.hpp"

#include <variant>

namespace mpc::sequencer
{
    struct UpdateBarLength
    {
        int barIndex;
        Tick length;
    };
    struct UpdateBarLengths
    {
        std::array<Tick, Mpc2000XlSpecs::MAX_BAR_COUNT> barLengths;
    };

    using SequenceMessage = std::variant<UpdateBarLength, UpdateBarLengths>;
} // namespace mpc::sequencer
