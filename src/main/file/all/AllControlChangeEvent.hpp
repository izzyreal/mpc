#pragma once

#include "sequencer/EventState.hpp"

namespace mpc::file::all
{
    class AllControlChangeEvent
    {
        static constexpr int CONTROLLER_OFFSET = 5;
        static constexpr int AMOUNT_OFFSET = 6;

    public:
        static sequencer::EventState bytesToMpcEvent(const std::vector<char> &);
        static std::vector<char> mpcEventToBytes(const sequencer::EventState &);
    };
} // namespace mpc::file::all
