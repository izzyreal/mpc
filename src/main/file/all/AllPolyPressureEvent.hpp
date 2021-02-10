#pragma once

#include <vector>

namespace mpc::sequencer {
class Event;
}

namespace mpc::file::all {

class AllPolyPressureEvent
{
    
private:
    static int NOTE_OFFSET;
    static int AMOUNT_OFFSET;
    
public:
    mpc::sequencer::Event* event;
    std::vector<char> saveBytes;
    
public:
    AllPolyPressureEvent(const std::vector<char>& ba);
    AllPolyPressureEvent(mpc::sequencer::Event* e);
};
}
