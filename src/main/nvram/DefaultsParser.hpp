#pragma once

#include "mpc_fs.hpp"

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
        static file::all::Defaults AllDefaultsFromFile(Mpc &mpc, fs::path file);
        std::vector<char> saveBytes;
        std::vector<char> getBytes();

        DefaultsParser(Mpc &mpc);
    };
} // namespace mpc::nvram
