#pragma once

#include "IntTypes.hpp"

#include <cstdint>
#include <variant>

namespace mpc::sequencer
{
    struct UpdateSequence
    {
    };

    using SequenceMessage =
        std::variant<UpdateSequence>;
} // namespace mpc::sequencer
