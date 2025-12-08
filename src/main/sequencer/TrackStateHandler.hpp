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

        void applyClearEvents(const RemoveEvents &, SequencerState &) const;
        void applyUpdateEvents(const UpdateEvents &, SequencerState &) const;
        void applyRemoveEvent(const RemoveEvent &, SequencerState &) const;
        void applyUpdateEventTick(const UpdateEventTick &,
                                  SequencerState &) const;
        void applyRemoveDoubles(const RemoveDoubles &, SequencerState &) const;
    };
} // namespace mpc::sequencer
