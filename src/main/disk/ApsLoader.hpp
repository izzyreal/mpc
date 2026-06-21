#pragma once

#include <memory>
#include <string>

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
    class ApsLoader
    {
    public:
        static void load(Mpc &, const std::shared_ptr<MpcFile> &,
                         bool headless);
    };
} // namespace mpc::disk
