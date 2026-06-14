#pragma once

#include "mpc_types.hpp"

#include <memory>
#include <string>
#include <vector>

namespace mpc
{
    class Mpc;
}

namespace mpc::disk
{
    class MpcFile;
}

namespace mpc::sampler
{
    class Program;
    class Sampler;
}

namespace mpc::file::kaitai
{
    class PgmIo
    {
    public:
        static program_or_error loadProgram(
            mpc::Mpc &,
            const std::shared_ptr<disk::MpcFile> &,
            const std::shared_ptr<sampler::Program> &,
            std::vector<std::string> &soundNames);

        static std::vector<char> saveProgram(
            sampler::Program &,
            const std::weak_ptr<sampler::Sampler> &);
    };
}
