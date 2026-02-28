#pragma once

#include "mpc_fs.hpp"

namespace mpc
{
    class Mpc;
}

namespace mpc::disk
{
    class AkaiFileRenamer
    {
    public:
        static void renameFilesInDirectory(Mpc &, const mpc_fs::path &p);
    };
} // namespace mpc::disk