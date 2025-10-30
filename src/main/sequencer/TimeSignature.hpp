#pragma once

namespace mpc::sequencer
{
    class TimeSignature
    {
    private:
        int numerator{};
        int denominator{};

    public:
        TimeSignature() = default;
        TimeSignature(const TimeSignature &other);
        bool operator!=(const TimeSignature &other) const
        {
            return numerator != other.numerator ||
                   denominator != other.denominator;
        }
        bool operator==(const TimeSignature &other) const
        {
            return numerator == other.numerator &&
                   denominator == other.denominator;
        }
        void setNumerator(int i);
        int getNumerator();

        void setDenominator(int i);
        int getDenominator();

        void increase();
        void decrease();
    };
} // namespace mpc::sequencer
