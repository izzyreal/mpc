#pragma once

#include "sequencer/SequencerState.hpp"
#include "sequencer/SequencerStateView.hpp"
#include "sequencer/SequencerMessage.hpp"

#include "concurrency/AtomicStateExchange.hpp"
#include "concurrency/FreeList.hpp"
#include "concurrency/SpinLock.hpp"

#include <concurrentqueue.h>

#include <functional>
#include <memory>

namespace mpc::sequencer
{
    class Sequencer;
    class TransportStateHandler;
    class SequenceStateHandler;
    class TrackStateHandler;
    class SongStateHandler;
    class SequenceStateView;

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

        void insertAcquiredEvent(TrackState &track, EventData *e) const;

        EventData *acquireEvent() const;

        std::array<std::array<concurrency::SpinLock,
                              Mpc2000XlSpecs::TOTAL_TRACK_COUNT>,
                   Mpc2000XlSpecs::TOTAL_SEQUENCE_COUNT>
            trackLocks;

        void applyCopyBars(const CopyBars &) noexcept;

        EventData *recordNoteEventLive(SequenceIndex, TrackIndex, NoteNumber, Velocity);

        // Only to be used for note events that are being recorded while the
        // sequencer is running, i.e. due to live MIDI, mouse, keyboard or
        // other input.
        // For non-live note event recording, i.e. in the step editor and in the
        // MAIN screen when the sequencer is not running, use
        // finalizeNoteEventNonLive.
        void finalizeNoteEventLive(const EventData *,
                                   Tick noteOffPositionTicks);

        EventData *findRecordingNoteOnEvent(SequenceIndex, TrackIndex,
                                            NoteNumber);

        void processLiveNoteEventRecordingQueues(Tick currentPositionTicks,
                                                 const SequenceStateView &seq);

        void flushNoteCache();

    protected:
        void applyMessage(const SequencerMessage &msg) noexcept override;

    private:
        static constexpr uint8_t LIVE_NOTE_EVENT_RECORDING_CAPACITY = 127;

        Sequencer *sequencer;
        std::unique_ptr<TransportStateHandler> transportStateHandler;
        std::unique_ptr<SequenceStateHandler> sequenceStateHandler;
        std::unique_ptr<TrackStateHandler> trackStateHandler;
        std::unique_ptr<SongStateHandler> songStateHandler;

        moodycamel::ConcurrentQueue<EventData *> liveNoteOnEventRecordingQueue;
        moodycamel::ConcurrentQueue<EventData> liveNoteOffEventRecordingQueue;
        std::vector<EventData *> tempLiveNoteOnRecordingEvents;
        std::vector<EventData> tempLiveNoteOffRecordingEvents;

        void applyCopyEvents(const CopyEvents &) noexcept;

        ///////  Live event recording helpers

        void assignTicksToNoteOffs(int noteOffCount, Tick pos);
        static void correctNoteOnTick(EventData *noteOn, Tick pos,
                                      Tick correctedPos);
        void adjustNoteOffTiming(const EventData *noteOn, EventData &noteOff,
                                 Tick lastTick) const;
        bool applyNoteOffAdjustmentForNoteOn(EventData *noteOn,
                                             int noteOffCount, Tick lastTick);
        void finalizeRecordedNote(EventData *noteOn, const EventData &noteOff,
                                  Tick lastTick);

        Tick getCorrectedTickPos(Tick currentPositionTicks,
                                 const SequenceStateView &) const;
        ///////  End of live event recording helpers

        friend class TransportStateHandler;
        friend class SequenceStateHandler;
    };
} // namespace mpc::sequencer
