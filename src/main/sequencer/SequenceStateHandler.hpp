#pragma once

#include "Sequencer.hpp"
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
                          const SequenceMessage &,
                          bool autoRefreshPlaybackState = true);

    private:
        SequencerStateManager *manager;
        Sequencer *sequencer;

        void
        applyInsertBars(const InsertBars &, SequencerState &,
                        std::vector<std::function<void()>> &actions) noexcept;
    };
} // namespace mpc::sequencer
