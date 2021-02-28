#pragma once

#include <memory>
#include <vector>

namespace mpc::sequencer {
class Event;
}

namespace mpc::file::all {
class AllEvent
{
    
public:
    static const int TICK_BYTE1_OFFSET = 0;
    static const int TICK_BYTE2_OFFSET = 1;
    static const int TICK_BYTE3_OFFSET = 2;
    static std::vector<int> TICK_BYTE3_BIT_RANGE;
    
public:
    static const int TRACK_OFFSET = 3;
    static const int EVENT_ID_OFFSET = 4;
    
    static const char POLY_PRESSURE_ID = 0xA0;
    static const char CONTROL_CHANGE_ID = 0xB0;
    static const char PGM_CHANGE_ID = 0xC0;
    static const char CH_PRESSURE_ID = 0xD0;
    static const char PITCH_BEND_ID = 0xE0;
    static const char SYS_EX_ID = 0xF0;
    
public:
    static int readTick(const std::vector<char>&);
    static void writeTick(std::vector<char>&, int);
    
    static std::shared_ptr<mpc::sequencer::Event> bytesToMpcEvent(const std::vector<char>&);
    static std::vector<char> mpcEventToBytes(std::shared_ptr<mpc::sequencer::Event>);
    
};
}
