#pragma once

#include "sequencer/SequencerState.hpp"
#include "sequencer/SequencerStateView.hpp"
#include "sequencer/SequencerMessage.hpp"
#include "concurrency/AtomicStateExchange.hpp"
#include "concurrency/FreeList.hpp"

#include <functional>
#include <memory>

namespace mpc::sequencer
{
    class Sequencer;
    class TransportStateHandler;
    class SequenceStateHandler;
    class TrackStateHandler;

    struct TrackLock
    {
        std::atomic<bool> flag{false};

        bool try_acquire()
        {
            bool expected = false;
            return flag.compare_exchange_strong(expected, true,
                                                std::memory_order_acquire,
                                                std::memory_order_relaxed);
        }

        void release()
        {
            flag.store(false, std::memory_order_release);
        }

        void acquire()
        {
            for (;;)
            {
                if (try_acquire())
                {
                    return;
                }
                std::this_thread::yield();
            }
        }
    };

    class SequencerStateManager final
        : public concurrency::AtomicStateExchange<
              SequencerState, SequencerStateView, SequencerMessage>
    {
    public:
        explicit SequencerStateManager(Sequencer *);
        ~SequencerStateManager() override;

        using EventStateFreeList =
            concurrency::FreeList<EventData,
                                  Mpc2000XlSpecs::GLOBAL_EVENT_CAPACITY>;

        std::shared_ptr<EventStateFreeList> pool;

        void returnEventToPool(EventData *e) const;

        static void insertAcquiredEvent(TrackState &track, EventData *e);

        EventData *acquireEvent() const;

        std::array<std::array<TrackLock, Mpc2000XlSpecs::TOTAL_TRACK_COUNT>,
                   Mpc2000XlSpecs::TOTAL_SEQUENCE_COUNT>
            trackLocks;

        void applyCopyBars(const CopyBars &) noexcept;

    protected:
        void applyMessage(const SequencerMessage &msg) noexcept override;

    private:
        Sequencer *sequencer;
        std::unique_ptr<TransportStateHandler> transportStateHandler;
        std::unique_ptr<SequenceStateHandler> sequenceStateHandler;
        std::unique_ptr<TrackStateHandler> trackStateHandler;

        void applyCopyEvents(const CopyEvents &) noexcept;

        friend class TransportStateHandler;
        friend class SequenceStateHandler;
    };
} // namespace mpc::sequencer
