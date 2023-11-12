#pragma once

#include "../mpc_fs.hpp"

namespace mpc { class Mpc; }

namespace mpc::disk {
    class AkaiFileRenamer {
    public:
        static void renameFilesInDirectory(mpc::Mpc&, const fs::path& p);
    };
}