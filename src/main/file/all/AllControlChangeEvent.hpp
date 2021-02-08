#pragma once

#include <vector>

namespace mpc::sequencer {
class Event;
}

namespace mpc::file::all {

class AllControlChangeEvent
{
    
private:
    static int CONTROLLER_OFFSET;
    static int AMOUNT_OFFSET;
    
public:
    mpc::sequencer::Event* event;
    std::vector<char> saveBytes;
    
public:
    AllControlChangeEvent(const std::vector<char>& ba);
    AllControlChangeEvent(mpc::sequencer::Event* e);
};
}
