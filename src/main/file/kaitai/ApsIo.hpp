#pragma once

#include <functional>
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

namespace mpc::file::kaitai
{
    class ApsIo
    {
    public:
        static void load(mpc::Mpc &, const std::shared_ptr<disk::MpcFile> &, bool headless);
        static std::vector<char> save(mpc::Mpc &, const std::string &apsName);
    };
}
