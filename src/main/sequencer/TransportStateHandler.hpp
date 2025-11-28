#pragma once

#include "Sequencer.hpp"
#include "TransportMessage.hpp"

namespace mpc::sequencer
{
    struct TransportState;

    class SequencerStateManager;
    class Sequencer;

    class TransportStateHandler final
    {
    public:
        explicit TransportStateHandler(SequencerStateManager *, Sequencer *);
        ~TransportStateHandler();

        void applyMessage(TransportState &,
                          const TransportMessage &,
                          bool autoRefreshPlaybackState = true);

        void applyPlayMessage(TransportState &) noexcept;
        void applyStopMessage(TransportState &) noexcept;

    private:
        SequencerStateManager *manager;
        Sequencer *sequencer;
    };
} // namespace mpc::sequencer
