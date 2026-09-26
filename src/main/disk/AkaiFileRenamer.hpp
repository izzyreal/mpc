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
        // Checked form for operation-owned I/O; errors propagate to the owner.
        static void renameFilesInDirectory(const mpc_fs::path &p,
                                           const mpc_fs::path &tempRoot);
    };
} // namespace mpc::disk