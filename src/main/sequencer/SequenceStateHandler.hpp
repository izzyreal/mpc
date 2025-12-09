#pragma once

#include "SequenceMessage.hpp"
#include "utils/SimpleAction.hpp"

namespace mpc::sequencer
{
    struct SequencerState;

    class SequencerStateManager;
    class Sequencer;

    class SequenceStateHandler final
    {
    public:
        explicit SequenceStateHandler(SequencerStateManager *, Sequencer *);
        ~SequenceStateHandler();

        void applyMessage(SequencerState &,
                          std::vector<utils::SimpleAction> &actions,
                          const SequenceMessage &) const;
    private:
        SequencerStateManager *manager;
        Sequencer *sequencer;

        void applyInsertBars(const InsertBars &, SequencerState &,
                             std::vector<utils::SimpleAction> &) const noexcept;

        void applyUpdateSequenceEvents(const UpdateSequenceEvents &m,
                                       SequencerState &state) const;
    };
} // namespace mpc::sequencer
