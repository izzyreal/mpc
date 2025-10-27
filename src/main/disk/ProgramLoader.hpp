#pragma once

#include <memory>
#include <string>
#include "mpc_types.hpp"

namespace mpc
{
    class Mpc;
}

namespace mpc::sampler
{
    class Program;
}

namespace mpc::disk
{
    class MpcFile;
}

namespace mpc::disk
{
    class ProgramLoader
    {
    public:
        static program_or_error
        loadProgram(mpc::Mpc &, std::shared_ptr<mpc::disk::MpcFile>,
                    std::shared_ptr<mpc::sampler::Program>);

    private:
        static void showLoadingSoundNamePopup(mpc::Mpc &mpc, std::string name,
                                              std::string ext, int sampleSize);
        static void notFound(mpc::Mpc &, std::string soundFileName);
    };
} // namespace mpc::disk
