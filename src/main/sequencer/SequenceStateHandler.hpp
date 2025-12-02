#pragma once

#include "SequenceMessage.hpp"

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
                          std::vector<std::function<void()>> &actions,
                          const SequenceMessage &) const;

    private:
        SequencerStateManager *manager;
        Sequencer *sequencer;

        void applyInsertBars(
            const InsertBars &, const SequencerState &,
            std::vector<std::function<void()>> &actions) const noexcept;
    };
} // namespace mpc::sequencer
