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
        static void renameFilesInDirectory(Mpc &, const fs::path &p);
    };
} // namespace mpc::disk