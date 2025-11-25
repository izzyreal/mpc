#pragma once

#include "IntTypes.hpp"

#include <variant>
#include <array>
#include <functional>

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

    struct UpdateTimeSignature
    {
        int barIndex;
        TimeSignature timeSignature;
        std::function<void()> onComplete = []{};
    };

    using SequenceMessage = std::variant<UpdateBarLength, UpdateBarLengths, UpdateTimeSignature>;
} // namespace mpc::sequencer
