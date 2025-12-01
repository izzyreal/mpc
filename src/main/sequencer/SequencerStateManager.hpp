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

    class SequencerStateManager final
        : public concurrency::AtomicStateExchange<
              SequencerState, SequencerStateView, SequencerMessage>
    {
    public:
        explicit SequencerStateManager(Sequencer *);
        ~SequencerStateManager() override;

        using EventStateFreeList = concurrency::FreeList<EventData, Mpc2000XlSpecs::GLOBAL_EVENT_CAPACITY>;

        std::shared_ptr<EventStateFreeList> pool;

        void returnEventToPool(EventData* e) const;

        void freeEvent(EventData*& head, EventData* e) const;

        static void insertEvent(TrackState& track, EventData* e,
                 bool allowMultipleNoteEventsWithSameNoteOnSameTick);

        EventData* acquireEvent() const;

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
