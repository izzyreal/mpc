#include "sequencer/TimeSignature.hpp"

using namespace mpc::sequencer;

void TimeSignature::increase()
{
    switch (denominator)
    {
        case 4:
        case 8:
        case 16:
        {
            if (numerator != 16)
            {
                ++numerator;
                break;
            }
            numerator = TimeSigNumerator(1);
            denominator *= 2;
            break;
        }
        case 32:
            if (numerator != 32)
            {
                ++numerator;
                break;
            }
        default:;
    }
}

void TimeSignature::decrease()
{
    switch (denominator)
    {
        case 4:
            if (numerator != 1)
            {
                --numerator;
            }
            break;
        case 8:
        case 16:
        case 32:
        {
            if (numerator == 1)
            {
                numerator = TimeSigNumerator(16);
                denominator /= 2;
                break;
            }
            --numerator;
            break;
        }
        default:;
    }
}
