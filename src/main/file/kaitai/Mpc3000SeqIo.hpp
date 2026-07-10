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

namespace mpc::file::kaitai
{
    class Mpc3000SeqIo
    {
    public:
        static sequence_or_error loadBytes(
            mpc::Mpc &,
            const std::vector<char> &,
            const std::string &fileNameWithoutExtension);
        static sequence_or_error
        load(mpc::Mpc &, const std::shared_ptr<disk::MpcFile> &);
    };
} // namespace mpc::file::kaitai
