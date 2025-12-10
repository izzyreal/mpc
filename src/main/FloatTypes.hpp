#pragma once
#include "MpcSpecs.hpp"

namespace mpc
{
    using PositionQuarterNotes = double;

    constexpr PositionQuarterNotes NoPositionQuarterNotes = -1;

    using Tempo = double;

    constexpr Tempo MinTempo = Mpc2000XlSpecs::MIN_TEMPO;
    constexpr Tempo MaxTempo = Mpc2000XlSpecs::MAX_TEMPO;
    constexpr Tempo DefaultTempo = Mpc2000XlSpecs::DEFAULT_TEMPO;

} // namespace mpc