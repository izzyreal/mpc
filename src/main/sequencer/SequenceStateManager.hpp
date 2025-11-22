#pragma once

#include "sequencer/SequenceState.hpp"
#include "sequencer/SequenceStateView.hpp"
#include "sequencer/SequenceMessage.hpp"
#include "concurrency/AtomicStateExchange.hpp"

namespace mpc::sequencer
{
    class Sequence;

    class SequenceStateManager final
        : public concurrency::AtomicStateExchange<
              SequenceState, SequenceStateView, SequenceMessage>
    {
    public:
        explicit SequenceStateManager(Sequence *);

    protected:
        void applyMessage(const SequenceMessage &msg) noexcept override;

    private:
        Sequence *sequence;
    };
} // namespace mpc::sequencer
