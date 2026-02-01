#pragma once

#include "TrackMessage.hpp"

namespace mpc::sequencer
{
    struct SequencerState;

    class SequencerStateManager;
    class SequenceStateHandler;
    class Sequencer;

    class TrackStateHandler final
    {
    public:
        explicit TrackStateHandler(SequencerStateManager *, Sequencer *);
        ~TrackStateHandler();

        void applyMessage(SequencerState &, const TrackMessage &) const;
        void applyRemoveEvents(const RemoveEvents &, SequencerState &) const;

    private:
        SequencerStateManager *manager;
        Sequencer *sequencer;

        void applyRemoveEvent(const RemoveEvent &, SequencerState &) const;
        void applyUpdateEventTick(const UpdateEventTick &,
                                  SequencerState &) const;
        void applyRemoveDoubles(const RemoveDoubles &, SequencerState &) const;

        friend class SequenceStateHandler;
    };
} // namespace mpc::sequencer
