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
        void myApplyMessage(const NonRtSequencerMessage &msg, bool autoRefreshPlaybackState = true) noexcept;

    private:
        const std::function<SampleRate()> getSampleRate;
        NonRtSequencerStateWorker *worker;
        Sequencer *sequencer;

        void applyPlayMessage() noexcept;
        void applyStopMessage() noexcept;
        void applyInsertBars(const InsertBars&) noexcept;
        void applyCopyEvents(const CopyEvents&) noexcept;
    };
} // namespace mpc::sequencer
