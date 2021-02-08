#pragma once

#include <vector>

namespace mpc::sequencer {
class Event;
}

namespace mpc::file::all {
class AllSysExEvent
{
    
private:
    static int CHUNK_HEADER_ID_OFFSET;
    static int BYTE_COUNT_OFFSET;
    static int DATA_OFFSET;
    static int MIX_TERMINATOR_ID_OFFSET;
    static int DATA_HEADER_ID_OFFSET;
    static char HEADER_ID;
    static char  DATA_TERMINATOR_ID;
    static char CHUNK_TERMINATOR_ID;
    static int MIXER_SIGNATURE_OFFSET;
    static std::vector<char> MIXER_SIGNATURE;
    static int MIXER_PARAMETER_OFFSET;
    static int MIXER_PAD_OFFSET;
    static int MIXER_VALUE_OFFSET;
    
public:
    std::vector<char> sysexLoadData{};
    mpc::sequencer::Event* event {  };
    std::vector<char> saveBytes{};
    
public:
    AllSysExEvent(const std::vector<char>& ba);
    AllSysExEvent(mpc::sequencer::Event* e);
};
}
