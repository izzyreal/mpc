#pragma once

#include "sequencer/SequencerState.hpp"
#include "sequencer/SequencerStateView.hpp"
#include "sequencer/SequencerMessage.hpp"
#include "concurrency/AtomicStateExchange.hpp"

namespace mpc::sequencer
{
    class Sequencer;

    class SequencerStateManager
        : public concurrency::AtomicStateExchange<
              SequencerState, SequencerStateView, SequencerMessage>
    {
    public:
        SequencerStateManager(Sequencer *);

        void enqueue(SequencerMessage &&msg) const noexcept override;

    protected:
        void applyMessage(const SequencerMessage &msg) noexcept override;

    private:
        Sequencer *sequencer;

        void applyPlayMessage(bool fromStart) noexcept;
    };
} // namespace mpc::sequencer
