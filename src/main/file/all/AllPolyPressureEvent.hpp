#pragma once

#include "performance/Sequence.hpp"

namespace mpc::file::all
{
    class AllPolyPressureEvent
    {
        static constexpr int NOTE_OFFSET = 5;
        static constexpr int AMOUNT_OFFSET = 6;
    public:
        static performance::Event
        bytesToMpcEvent(const std::vector<char> &);
        static std::vector<char>
        mpcEventToBytes(const performance::Event &);
    };
} // namespace mpc::file::all
