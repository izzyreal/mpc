#pragma once

#include "sequencer/NonRtSequencerState.hpp"
#include "sequencer/NonRtSequencerStateView.hpp"
#include "sequencer/NonRtSequencerMessage.hpp"
#include "concurrency/AtomicStateExchange.hpp"

namespace mpc::sequencer
{
    class TrackEvent;

    class NonRtSequencerStateManager final
        : public concurrency::AtomicStateExchange<
              NonRtSequencerState, NonRtSequencerStateView, NonRtSequencerMessage>
    {
    public:
        explicit NonRtSequencerStateManager();
        ~NonRtSequencerStateManager() override;

    protected:
        void applyMessage(const NonRtSequencerMessage &msg) noexcept override;
    };
} // namespace mpc::sequencer
