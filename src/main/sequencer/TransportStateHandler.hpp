#pragma once

#include "Sequencer.hpp"
#include "TransportMessage.hpp"

namespace mpc::sequencer
{
    struct TransportState;
    class SequenceStateView;

    class SequencerStateManager;
    class Sequencer;

    class TransportStateHandler final
    {
    public:
        explicit TransportStateHandler(SequencerStateManager *, Sequencer *);
        ~TransportStateHandler();
        void installCountIn(TransportState &state, bool fromStart) const;
        void installCountIn(TransportState &state, bool fromStart,
                            const SequenceStateView &sequence) const;

        void applyMessage(TransportState &, const TransportMessage &);

        void applyPlaySequence(TransportState &) const noexcept;
        void applyPlaySequence(TransportState &, SequenceIndex) const noexcept;
        void applyPlaySong(TransportState &) const noexcept;
        void applyStopSequence(TransportState &) const noexcept;
        void applyStopSong(TransportState &) const noexcept;

    private:
        SequencerStateManager *manager;
        Sequencer *sequencer;
    };
} // namespace mpc::sequencer
