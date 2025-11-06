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
        loadProgram(mpc::Mpc &, const std::shared_ptr<mpc::disk::MpcFile> &,
                    const std::shared_ptr<mpc::sampler::Program> &);

    private:
        static void showLoadingSoundNamePopup(mpc::Mpc &mpc,
                                              const std::string &name,
                                              const std::string &ext,
                                              int sampleSize);
        static void notFound(mpc::Mpc &, const std::string &soundFileName);
    };
} // namespace mpc::disk
