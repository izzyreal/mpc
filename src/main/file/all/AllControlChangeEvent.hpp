#pragma once

#include <vector>
#include <memory>

namespace mpc::sequencer
{
    class ControlChangeEvent;
}

namespace mpc::file::all
{

    class AllControlChangeEvent
    {

        const static int CONTROLLER_OFFSET = 5;
        const static int AMOUNT_OFFSET = 6;

    public:
        static std::shared_ptr<sequencer::ControlChangeEvent>
        bytesToMpcEvent(const std::vector<char> &);
        static std::vector<char>
        mpcEventToBytes(const std::shared_ptr<sequencer::ControlChangeEvent> &);
    };
} // namespace mpc::file::all
