#pragma once

#include "sequencer/TimeSignature.hpp"

#include "IntTypes.hpp"
#include "MpcSpecs.hpp"

#include <array>

namespace mpc::sequencer
{
    struct SequenceState
    {
        std::array<TimeSignature, Mpc2000XlSpecs::MAX_BAR_COUNT> timeSignatures;
        std::array<Tick, Mpc2000XlSpecs::MAX_BAR_COUNT> barLengths;
    };
} // namespace mpc::sequencer