#pragma once

#include "sequencer/EventData.hpp"

namespace mpc::file::all
{
    class AllControlChangeEvent
    {
        static constexpr int CONTROLLER_OFFSET = 5;
        static constexpr int AMOUNT_OFFSET = 6;

    public:
        static sequencer::EventData bytesToMpcEvent(const std::vector<char> &);
        static std::vector<char> mpcEventToBytes(const sequencer::EventData &);
    };
} // namespace mpc::file::all
