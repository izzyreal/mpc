#pragma once
#include "IntTypes.hpp"

namespace mpc::sequencer
{
    struct TimeSignature
    {
        TimeSigNumerator numerator{DefaultTimeSigNumerator};
        TimeSigDenominator denominator{DefaultTimeSigDenominator};

        bool operator==(TimeSignature const &other) const
        {
            return numerator == other.numerator &&
                   denominator == other.denominator;
        }

        void increase();
        void decrease();
    };
} // namespace mpc::sequencer
