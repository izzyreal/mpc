#pragma once

#include "IntTypes.hpp"

#include <variant>

namespace mpc::sequencer
{
    struct SetTimeInSamples
    {
        TimeInSamples timeInSamples;
    };

    using SequencerMessage = std::variant<SetTimeInSamples>;
} // namespace mpc::sequencer
