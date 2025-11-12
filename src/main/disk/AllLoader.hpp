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
        static void loadEverythingFromFile(Mpc &, MpcFile *);
        static void loadEverythingFromAllParser(Mpc &, file::all::AllParser &);
        static std::vector<std::shared_ptr<sequencer::Sequence>>
        loadOnlySequencesFromFile(Mpc &, MpcFile *);
    };
} // namespace mpc::disk
