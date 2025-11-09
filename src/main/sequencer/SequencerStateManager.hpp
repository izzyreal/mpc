#pragma once

#include "sequencer/SequencerState.hpp"
#include "sequencer/SequencerStateView.hpp"
#include "sequencer/SequencerMessage.hpp"
#include "concurrency/AtomicStateExchange.hpp"

namespace mpc::sequencer
{
    class SequencerStateManager : public mpc::concurrency::AtomicStateExchange<SequencerState, SequencerStateView, SequencerMessage>
    {
    public:
        SequencerStateManager();
    protected:
        void applyMessage(const SequencerMessage &msg) noexcept override;
    };
}
