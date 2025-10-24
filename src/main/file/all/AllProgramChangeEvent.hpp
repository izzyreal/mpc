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

    private:
        const static int PROGRAM_OFFSET = 5;

    public:
        static std::shared_ptr<mpc::sequencer::ProgramChangeEvent> bytesToMpcEvent(const std::vector<char> &);
        static std::vector<char> mpcEventToBytes(std::shared_ptr<mpc::sequencer::ProgramChangeEvent>);
    };
} // namespace mpc::file::all
