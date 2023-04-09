#pragma once

#include "../mpc_fs.hpp"

namespace mpc::disk {
    class AkaiFileRenamer {
    public:
        static void renameFilesInDirectory(fs::path p);
    };
}