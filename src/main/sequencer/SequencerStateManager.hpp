#pragma once

#include "sequencer/SequencerState.hpp"
#include "sequencer/SequencerStateView.hpp"
#include "sequencer/SequencerMessage.hpp"
#include "concurrency/AtomicStateExchange.hpp"

#include <functional>
#include <memory>

namespace mpc::sequencer
{
    class SequencerStateWorker;
    class TrackEvent;
    class Sequencer;
    class TransportStateHandler;
    class SequenceStateHandler;

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
        std::unique_ptr<TransportStateHandler> transportStateHandler;
        std::unique_ptr<SequenceStateHandler> sequenceStateHandler;

        void applyCopyEvents(const CopyEvents &) noexcept;

        friend class TransportStateHandler;
        friend class SequenceStateHandler;
    };
} // namespace mpc::sequencer
