#pragma once

#include <memory>
#include <vector>

namespace mpc::sequencer
{
    class ProgramChangeEvent;
}

namespace mpc::file::all
{
    class AllProgramChangeEvent
    {

        const static int PROGRAM_OFFSET = 5;

    public:
        static std::shared_ptr<sequencer::ProgramChangeEvent>
        bytesToMpcEvent(const std::vector<char> &);
        static std::vector<char>
        mpcEventToBytes(const std::shared_ptr<sequencer::ProgramChangeEvent> &);
    };
} // namespace mpc::file::all
