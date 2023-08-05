#pragma once

#include <Observer.hpp>

namespace mpc::sequencer {

    class TimeSignature
            : public Observable
    {

    private:
        int numerator;
        int denominator;

    public:
        void setNumerator(int i);

        int getNumerator();

        void setDenominator(int i);

        int getDenominator();

        void increase();

        void decrease();
    };
}
