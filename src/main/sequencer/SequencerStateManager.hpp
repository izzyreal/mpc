#pragma once

#include "sequencer/SequencerState.hpp"
#include "sequencer/SequencerStateView.hpp"
#include "sequencer/SequencerMessage.hpp"
#include "concurrency/AtomicStateExchange.hpp"

namespace mpc::sequencer
{
    class SequencerStateManager
        : public concurrency::AtomicStateExchange<
              SequencerState, SequencerStateView, SequencerMessage>
    {
    public:
        SequencerStateManager();

        void enqueue(SequencerMessage &&msg) const noexcept override;

    protected:
        void applyMessage(const SequencerMessage &msg) noexcept override;
    };
} // namespace mpc::sequencer
