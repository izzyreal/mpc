#pragma once

#include <vector>

namespace mpc::file::all
{
    class Bar;

    class Bar
    {

    public:
        int ticksPerBeat;
        int lastTick;
        int barLength;
        std::vector<char> saveBytes;

    public:
        int getTicksPerBeat() const;
        int getDenominator() const;
        int getNumerator() const;
        int getLastTick() const;

    public:
        std::vector<char> &getBytes();

        Bar(const std::vector<char> &bytes, Bar *previousBar);
        Bar(int ticksPerBeat, int lastTick);
    };
} // namespace mpc::file::all
