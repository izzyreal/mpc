#pragma once

#include "sequencer/SequencerState.hpp"
#include "sequencer/SequencerStateView.hpp"
#include "sequencer/SequencerMessage.hpp"
#include "concurrency/AtomicStateExchange.hpp"

namespace mpc::sequencer
{
    class Sequencer;

    class SequencerStateManager final
        : public concurrency::AtomicStateExchange<
              SequencerState, SequencerStateView, SequencerMessage>
    {
    public:
        explicit SequencerStateManager(Sequencer *);
        ~SequencerStateManager() override;

    protected:
        void applyMessage(const SequencerMessage &msg) noexcept override;

    private:
        Sequencer *sequencer;

        void applyPlayMessage(bool fromStart) const noexcept;
    };
} // namespace mpc::sequencer
