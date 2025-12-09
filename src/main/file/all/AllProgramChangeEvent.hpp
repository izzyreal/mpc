#pragma once

#include "sequencer/EventData.hpp"

#include <vector>

namespace mpc::file::all
{
    class AllProgramChangeEvent
    {
        static constexpr int PROGRAM_OFFSET = 5;

    public:
        static sequencer::EventData bytesToMpcEvent(const std::vector<char> &);
        static std::vector<char> mpcEventToBytes(const sequencer::EventData &);
    };
} // namespace mpc::file::all
