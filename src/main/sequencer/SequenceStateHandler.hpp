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

        void applyInsertBars(const InsertBars &,
                             SequencerState &) const noexcept;

        void applyUpdateSequenceEvents(const UpdateSequenceEvents &m,
                                       SequencerState &) const;

        void applyDeleteTrack(const DeleteTrack &, SequencerState &) const;

        void appleEraseEvents(const EraseEvents &, SequencerState &) const;
    };
} // namespace mpc::sequencer
