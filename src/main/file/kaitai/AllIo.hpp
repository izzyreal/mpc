#pragma once

#include "IntTypes.hpp"
#include "mpc_types.hpp"

#include <memory>
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
    struct SequenceMetaInfo;
}

namespace mpc::file::kaitai
{
    class AllIo
    {
    public:
        static void loadEverything(mpc::Mpc &, mpc::disk::MpcFile *);
        static sequence_meta_infos_or_error loadSequenceMetaInfos(mpc::Mpc &, mpc::disk::MpcFile *);
        static std::shared_ptr<mpc::sequencer::Sequence> loadOneSequence(
            mpc::Mpc &,
            mpc::disk::MpcFile *,
            mpc::SequenceIndex sourceIndexInAllFile,
            mpc::SequenceIndex destIndexInMpcMemory);
        static std::vector<char> save(mpc::Mpc &);
    };
}
