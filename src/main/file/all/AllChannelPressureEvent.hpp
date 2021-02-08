#pragma once

#include <vector>

namespace mpc::sequencer {
class Event;
}

namespace mpc::file::all {
class AllChannelPressureEvent
{
    
private:
    static int AMOUNT_OFFSET;
    
public:
    mpc::sequencer::Event* event {  };
    std::vector<char> saveBytes{};
    
public:
    AllChannelPressureEvent(const std::vector<char>& ba);
    AllChannelPressureEvent(mpc::sequencer::Event* e);
};
}
