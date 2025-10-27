#include "sequencer/TimeSignature.hpp"

using namespace mpc::sequencer;

TimeSignature::TimeSignature(const TimeSignature &other)
    : numerator(other.numerator), denominator(other.denominator)
{
}

void TimeSignature::setNumerator(int i)
{
    numerator = i;
}

int TimeSignature::getNumerator()
{
    return numerator;
}

void TimeSignature::setDenominator(int i)
{
    denominator = i;
}

int TimeSignature::getDenominator()
{
    return denominator;
}

void TimeSignature::increase()
{
    switch (denominator)
    {
        case 4:
        case 8:
        case 16:
            if (numerator != 16)
            {
                ++numerator;
                break;
            }
            else
            {
                numerator = 1;
                denominator *= 2;
                break;
            }
        case 32:
            if (numerator != 32)
            {
                ++numerator;
                break;
            }
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
            if (numerator == 1)
            {
                numerator = 16;
                denominator /= 2;
                break;
            }
            else
            {
                --numerator;
                break;
            }
    }
}
