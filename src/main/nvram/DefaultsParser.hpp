#pragma once

#include "../mpc_fs.hpp"

#include <vector>

namespace mpc
{
    class Mpc;
}

namespace mpc::file::all
{
    class Defaults;
}

namespace mpc::nvram
{

    class DefaultsParser
    {

    public:
        static mpc::file::all::Defaults AllDefaultsFromFile(mpc::Mpc &mpc, fs::path file);
        std::vector<char> saveBytes;
        std::vector<char> getBytes();

        DefaultsParser(mpc::Mpc &mpc);
    };
} // namespace mpc::nvram
