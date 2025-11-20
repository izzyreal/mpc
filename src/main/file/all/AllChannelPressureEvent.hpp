#pragma once

#include "performance/Sequence.hpp"

namespace mpc::file::all
{
    class AllChannelPressureEvent
    {
        static constexpr int AMOUNT_OFFSET = 5;
    public:
        static performance::Event bytesToMpcEvent(const std::vector<char> &);
        static std::vector<char> mpcEventToBytes(
            const performance::Event &);
    };
} // namespace mpc::file::all
