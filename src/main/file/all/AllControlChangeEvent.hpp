#pragma once

#include <vector>
#include <memory>

namespace mpc::sequencer {
class ControlChangeEvent;
}

namespace mpc::file::all {

class AllControlChangeEvent
{
    
private:
    const static int CONTROLLER_OFFSET = 5;
    const static int AMOUNT_OFFSET = 6;

public:
    static std::shared_ptr<mpc::sequencer::ControlChangeEvent> bytesToMpcEvent(const std::vector<char>&);
    static std::vector<char> mpcEventToBytes(std::shared_ptr<mpc::sequencer::ControlChangeEvent>);

};
}
