#pragma once

#include "sequencer/NonRtSequencerState.hpp"
#include "sequencer/NonRtSequencerStateView.hpp"
#include "sequencer/NonRtSequencerMessage.hpp"
#include "concurrency/AtomicStateExchange.hpp"

#include <functional>

namespace mpc::sequencer
{
    class NonRtSequencerStateWorker;
    class TrackEvent;
    class Sequencer;

    class NonRtSequencerStateManager final
        : public concurrency::AtomicStateExchange<
              NonRtSequencerState, NonRtSequencerStateView, NonRtSequencerMessage>
    {
    public:
        explicit NonRtSequencerStateManager(const std::function<SampleRate()> &getSampleRate, Sequencer *, NonRtSequencerStateWorker *);
        ~NonRtSequencerStateManager() override;

    protected:
        void applyMessage(const NonRtSequencerMessage &msg) noexcept override;

    private:
        const std::function<SampleRate()> getSampleRate;
        NonRtSequencerStateWorker *worker;
        Sequencer *sequencer;

        void applyPlayMessage(bool fromStart) noexcept;
        void applyStopMessage() noexcept;
    };
} // namespace mpc::sequencer
