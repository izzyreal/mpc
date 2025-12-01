#pragma once

#include "sequencer/EventData.hpp"

namespace mpc::file::all
{
    class AllPitchBendEvent
    {
        static constexpr int AMOUNT_OFFSET = 5;

    public:
        static sequencer::EventData bytesToMpcEvent(const std::vector<char> &);
        static std::vector<char> mpcEventToBytes(const sequencer::EventData &);
    };
} // namespace mpc::file::all
