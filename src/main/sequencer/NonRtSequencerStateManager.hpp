#pragma once

#include "sequencer/NonRtSequencerState.hpp"
#include "sequencer/NonRtSequencerStateView.hpp"
#include "sequencer/NonRtSequencerMessage.hpp"
#include "concurrency/AtomicStateExchange.hpp"

namespace mpc::sequencer
{
    class NonRtSequencerStateWorker;
    class TrackEvent;

    class NonRtSequencerStateManager final
        : public concurrency::AtomicStateExchange<
              NonRtSequencerState, NonRtSequencerStateView, NonRtSequencerMessage>
    {
    public:
        explicit NonRtSequencerStateManager(NonRtSequencerStateWorker *);
        ~NonRtSequencerStateManager() override;

    protected:
        void applyMessage(const NonRtSequencerMessage &msg) noexcept override;

    private:
        NonRtSequencerStateWorker *worker;
    };
} // namespace mpc::sequencer
