#pragma once

#include "TrackMessage.hpp"

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
                          std::vector<std::function<void()>> &actions,
                          const TrackMessage &) const;

    private:
        SequencerStateManager *manager;
        Sequencer *sequencer;
    };
} // namespace mpc::sequencer
