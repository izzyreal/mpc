#pragma once

#include "sequencer/SequencerState.hpp"
#include "sequencer/SequencerStateView.hpp"
#include "sequencer/SequencerMessage.hpp"
#include "concurrency/AtomicStateExchange.hpp"

#include <functional>

namespace mpc::sequencer
{
    class SequencerStateWorker;
    class TrackEvent;
    class Sequencer;
    class TransportStateHandler;

    class SequencerStateManager final
        : public concurrency::AtomicStateExchange<
              SequencerState, SequencerStateView, SequencerMessage>
    {
    public:
        explicit SequencerStateManager(
            const std::function<SampleRate()> &getSampleRate, Sequencer *,
            SequencerStateWorker *);
        ~SequencerStateManager() override;

    protected:
        void applyMessage(const SequencerMessage &msg) noexcept override;
        void myApplyMessage(const SequencerMessage &msg,
                            bool autoRefreshPlaybackState = true) noexcept;

    private:
        const std::function<SampleRate()> getSampleRate;
        SequencerStateWorker *worker;
        Sequencer *sequencer;

        void applyInsertBars(const InsertBars &) noexcept;
        void applyCopyEvents(const CopyEvents &) noexcept;

        friend class TransportStateHandler;
    };
} // namespace mpc::sequencer
