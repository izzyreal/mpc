#pragma once

#include "IntTypes.hpp"

#include "sequencer/SequenceMetaInfo.hpp"

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

namespace mpc::file::all
{
} // namespace mpc::file::all

namespace mpc::sequencer
{
    class Sequence;
}

namespace mpc::disk
{
    class AllLoader
    {
    public:
        static void loadEverythingFromFile(Mpc &, MpcFile *);
        static void loadEverythingFromBytes(Mpc &, const std::vector<char> &);
        static void loadEverythingFromCanonicalBytes(Mpc &, const std::vector<char> &);
        static std::shared_ptr<sequencer::Sequence>
        loadOneSequenceFromCanonicalBytes(Mpc &, const std::vector<char> &,
                                          SequenceIndex sourceIndexInAllFile,
                                          SequenceIndex destIndexInMpcMemory);
        static std::vector<sequencer::SequenceMetaInfo>
        loadSequenceMetaInfosFromFile(Mpc &, MpcFile *f);
        static std::shared_ptr<sequencer::Sequence>
        loadOneSequenceFromFile(Mpc &, MpcFile *,
                                SequenceIndex sourceIndexInAllFile,
                                SequenceIndex destIndexInMpcMemory);
    };
} // namespace mpc::disk
