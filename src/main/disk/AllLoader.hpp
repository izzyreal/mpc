#pragma once

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
    class AllParser;
    class AllSequence;
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
        static void loadEverythingFromFile(mpc::Mpc &, mpc::disk::MpcFile *);
        static void loadEverythingFromAllParser(mpc::Mpc &,
                                                mpc::file::all::AllParser &);
        static std::vector<std::shared_ptr<mpc::sequencer::Sequence>>
        loadOnlySequencesFromFile(mpc::Mpc &, mpc::disk::MpcFile *);
    };
} // namespace mpc::disk
