#pragma once

#include <memory>
#include <vector>

namespace mpc::sequencer
{
    class PolyPressureEvent;
}

namespace mpc::file::all
{

    class AllPolyPressureEvent
    {

        const static int NOTE_OFFSET = 5;
        const static int AMOUNT_OFFSET = 6;

    public:
        static std::shared_ptr<sequencer::PolyPressureEvent>
        bytesToMpcEvent(const std::vector<char> &);
        static std::vector<char>
        mpcEventToBytes(const std::shared_ptr<sequencer::PolyPressureEvent> &);
    };
} // namespace mpc::file::all
