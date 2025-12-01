#pragma once

#include "sequencer/EventData.hpp"

#include <memory>
#include <vector>

namespace mpc::sequencer
{
    class EventRef;
}

namespace mpc::file::all
{
    class AllEvent
    {
    public:
        static constexpr int TICK_BYTE1_OFFSET = 0;
        static constexpr int TICK_BYTE2_OFFSET = 1;
        static constexpr int TICK_BYTE3_OFFSET = 2;
        static std::vector<int> TICK_BYTE3_BIT_RANGE;

        static constexpr int TRACK_OFFSET = 3;
        static constexpr int EVENT_ID_OFFSET = 4;

        static constexpr char POLY_PRESSURE_ID = 0xA0;
        static constexpr char CONTROL_CHANGE_ID = 0xB0;
        static constexpr char PGM_CHANGE_ID = 0xC0;
        static constexpr char CH_PRESSURE_ID = 0xD0;
        static constexpr char PITCH_BEND_ID = 0xE0;
        static constexpr char SYS_EX_ID = 0xF0;

        static int readTick(const std::vector<char> &);
        static void writeTick(std::vector<char> &, int);

        static sequencer::EventData bytesToMpcEvent(const std::vector<char> &);
        static std::vector<char> mpcEventToBytes(const sequencer::EventData &);
    };
} // namespace mpc::file::all
