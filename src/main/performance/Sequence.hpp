#pragma once

#include "MpcSpecs.hpp"
#include "IntTypes.hpp"

#include <array>

namespace mpc::performance
{
    struct TimeSignature
    {
        TimeSigNumerator numerator{DefaultTimeSigNumerator};
        TimeSigDenominator denominator{DefaultTimeSigDenominator};
    };

    struct Sequence
    {
        std::array<TimeSignature, Mpc2000XlSpecs::MAX_BAR_COUNT> timeSignatures;
        std::array<Tick, Mpc2000XlSpecs::MAX_BAR_COUNT> barLengths;
    };
} // namespace mpc::performance