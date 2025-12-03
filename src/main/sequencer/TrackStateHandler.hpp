#pragma once

#include "TrackMessage.hpp"
#include "utils/SimpleAction.hpp"

namespace mpc::sequencer
{
    struct SequencerState;

    class SequencerStateManager;
    class Sequencer;

    class TrackStateHandler final
    {
    public:
        explicit TrackStateHandler(SequencerStateManager *, Sequencer *);
        ~TrackStateHandler();

        void applyMessage(SequencerState &,
                          std::vector<utils::SimpleAction> &actions,
                          const TrackMessage &) const;

    private:
        SequencerStateManager *manager;
        Sequencer *sequencer;
    };
} // namespace mpc::sequencer
