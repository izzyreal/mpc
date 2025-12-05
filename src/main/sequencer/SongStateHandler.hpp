#pragma once

#include "sequencer/SongMessage.hpp"

#include "utils/SimpleAction.hpp"

#include <vector>

namespace mpc::sequencer
{
    class SequencerState;

    class SongStateHandler final
    {
    public:
        explicit SongStateHandler();
        ~SongStateHandler();

        static void applyMessage(SequencerState &,
                          std::vector<utils::SimpleAction> &actions,
                          const SongMessage &);
    };
} // namespace mpc::sequencer