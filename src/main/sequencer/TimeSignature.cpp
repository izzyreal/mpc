#include "sequencer/TimeSignature.hpp"

using namespace mpc::sequencer;

TimeSignature::TimeSignature(const TimeSignature &other)
    : numerator(other.numerator), denominator(other.denominator)
{
}

void TimeSignature::setNumerator(const int i)
{
    numerator = i;
}

int TimeSignature::getNumerator() const
{
    return numerator;
}

void TimeSignature::setDenominator(const int i)
{
    denominator = i;
}

int TimeSignature::getDenominator() const
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
        {
            if (numerator != 16)
            {
                ++numerator;
                break;
            }
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
        {
            if (numerator == 1)
            {
                numerator = 16;
                denominator /= 2;
                break;
            }
            --numerator;
            break;
        }
    }
}
