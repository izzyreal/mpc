#pragma once

#include "IntTypes.hpp"

#include <variant>

namespace mpc::sequencer
{
    struct SetTimeInSamples
    {
        TimeInSamples timeInSamples;
    };

    using SequencerAudioMessage = std::variant<SetTimeInSamples>;
} // namespace mpc::sequencer
