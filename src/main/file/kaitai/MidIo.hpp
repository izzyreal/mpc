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

namespace mpc::sequencer
{
    class Sequence;
}

namespace mpc::file::kaitai
{
    class MidIo
    {
    public:
        static sequence_or_error loadBytes(
            mpc::Mpc &,
            const std::vector<char> &,
            const std::string &fileNameWithoutExtension);
        static sequence_or_error load(mpc::Mpc &, const std::shared_ptr<disk::MpcFile> &);
        static std::vector<char> saveBytes(const std::shared_ptr<sequencer::Sequence> &);
        static void save(const std::shared_ptr<sequencer::Sequence> &, const std::shared_ptr<std::ostream> &);
    };
}
