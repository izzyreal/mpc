#pragma once

#include <vector>
#include <memory>

namespace mpc::sequencer
{
    class PitchBendEvent;
}

namespace mpc::file::all
{
    class AllPitchBendEvent
    {

        const static int AMOUNT_OFFSET = 5;

    public:
        static std::shared_ptr<sequencer::PitchBendEvent>
        bytesToMpcEvent(const std::vector<char> &);
        static std::vector<char>
        mpcEventToBytes(const std::shared_ptr<sequencer::PitchBendEvent> &);
    };
} // namespace mpc::file::all
