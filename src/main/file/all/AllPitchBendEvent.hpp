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

    private:
        const static int AMOUNT_OFFSET = 5;

    public:
        static std::shared_ptr<mpc::sequencer::PitchBendEvent>
        bytesToMpcEvent(const std::vector<char> &);
        static std::vector<char>
            mpcEventToBytes(std::shared_ptr<mpc::sequencer::PitchBendEvent>);
    };
} // namespace mpc::file::all
