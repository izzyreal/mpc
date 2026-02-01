#include "sequencer/SequencerStateManager.hpp"

#include "sequencer/SequenceStateHandler.hpp"
#include "sequencer/SongStateHandler.hpp"
#include "sequencer/Track.hpp"
#include "sequencer/TrackStateHandler.hpp"
#include "sequencer/TrackStateView.hpp"
#include "lcdgui/Screens.hpp"
#include "lcdgui/screens/window/TimingCorrectScreen.hpp"
#include "sequencer/SequenceStateView.hpp"
#include "sequencer/Sequence.hpp"
#include "sequencer/TransportStateHandler.hpp"
#include "utils/VariantUtils.hpp"

#include <cstring>

using namespace mpc::sequencer;
using namespace mpc::concurrency;

SequencerStateManager::SequencerStateManager(Sequencer *sequencer)
    : AtomicStateExchange([](SequencerState &) {}), sequencer(sequencer)
{
    eventPool = std::make_shared<EventPool>();

    transportStateHandler =
        std::make_unique<TransportStateHandler>(this, sequencer);

    sequenceStateHandler =
        std::make_unique<SequenceStateHandler>(this, sequencer);

    trackStateHandler = std::make_unique<TrackStateHandler>(this, sequencer);

    songStateHandler = std::make_unique<SongStateHandler>();

    tempLiveNoteOnRecordingEvents.resize(LIVE_NOTE_EVENT_RECORDING_CAPACITY);
    tempLiveNoteOffRecordingEvents.resize(LIVE_NOTE_EVENT_RECORDING_CAPACITY);
}

SequencerStateManager::~SequencerStateManager() {}

void SequencerStateManager::returnEventToPool(EventData *e) const
{
    // Reset object to default "clean" state before returning it.
    e->resetToDefaultValues();

    // Return pointer to freelist
    eventPool->release(e);
}

EventData *SequencerStateManager::acquireEvent() const
{
    EventData *e = nullptr;
    if (!eventPool->acquire(e))
    {
        return nullptr;
    }

    new (e) EventData(); // placement-new calls resetToDefaultValues()
    return e;
}

void SequencerStateManager::applyMessage(const SequencerMessage &msg) noexcept
{
    auto visitor = Overload{
        [&](const TransportMessage &m)
        {
            transportStateHandler->applyMessage(activeState.transport, m);
        },
        [&](const SequenceMessage &m)
        {
            sequenceStateHandler->applyMessage(activeState, actions, m);
        },
        [&](const TrackMessage &m)
        {
            trackStateHandler->applyMessage(activeState, m);
        },
        [&](const SongMessage &m)
        {
            songStateHandler->applyMessage(activeState, actions, m);
        },
        [&](const SetNextSequenceIndex &m)
        {
            activeState.nextSequenceIndex = m.sequenceIndex;
        },
        [&](const SwitchToNextSequenceSudden &m)
        {
            constexpr bool setPositionTo0 = true;
            enqueue(SetSelectedSequenceIndex{m.sequenceIndex, setPositionTo0});
        },
        [&](const SetSelectedSequenceIndex &m)
        {
            activeState.selectedSequenceIndex = m.sequenceIndex;

            if (m.setPositionTo0)
            {
                activeState.transport.positionQuarterNotes = 0;
                applyMessageImmediate(
                    SyncTrackEventIndices{activeState.selectedSequenceIndex});
            }
        },
        [&](const SetSelectedSongIndex &m)
        {
            activeState.selectedSongIndex = m.songIndex;
            activeState.selectedSongStepIndex = MinSongStepIndex;

            if (m.sequenceIndexOfFirstStep != NoSequenceIndex)
            {
                activeState.selectedSequenceIndex = m.sequenceIndexOfFirstStep;
                activeState.transport.positionQuarterNotes = 0;
                applyMessage(
                    SyncTrackEventIndices{activeState.selectedSequenceIndex});
            }
        },
        [&](const SetSelectedSongStepIndex &m)
        {
            activeState.selectedSongStepIndex = m.songStepIndex;

            if (m.sequenceIndexForThisStep != NoSequenceIndex)
            {
                activeState.selectedSequenceIndex = m.sequenceIndexForThisStep;
                activeState.transport.positionQuarterNotes = 0;
                applyMessage(
                    SyncTrackEventIndices{activeState.selectedSequenceIndex});
            }
        },
        [&](const CopyBars &m)
        {
            applyCopyBars(m);
        },
        [&](const CopyEvents &m)
        {
            applyCopyEvents(m);
        },
        [&](const DeleteSequence &m)
        {
            activeState.sequences[m.sequenceIndex].initializeDefaults();
        },
        [&](const DeleteAllSequences &)
        {
            for (auto &seq : activeState.sequences)
            {
                seq.initializeDefaults();
            }
        },
        [&](const CopySequence &m)
        {
            auto &seqLock1 = sequenceLocks[m.sourceIndex];
            auto &seqLock2 = sequenceLocks[m.destIndex];

            if (!seqLock1.try_acquire() || !seqLock2.try_acquire())
            {
                enqueue(m);
                return;
            }

            const auto &source = activeState.sequences[m.sourceIndex];
            auto &dest = activeState.sequences[m.destIndex];
            dest.name.assign(source.name.data(), source.name.size());
            dest.lastBarIndex = source.lastBarIndex;
            dest.timeSignatures = source.timeSignatures;
            dest.barLengths = source.barLengths;
            dest.loopEnabled = source.loopEnabled;
            dest.firstLoopBarIndex = source.firstLoopBarIndex;
            dest.lastLoopBarIndex = source.lastLoopBarIndex;
            dest.initialTempo = source.initialTempo;
            dest.tempoChangeEnabled = source.tempoChangeEnabled;
            dest.used = source.used;

            for (int8_t idx = 0; idx < Mpc2000XlSpecs::TOTAL_TRACK_COUNT; ++idx)
            {
                const auto &t1 =
                    activeState.sequences[m.sourceIndex].tracks[idx];

                auto &t2 =
                    activeState.sequences[m.destIndex].tracks[idx];

                RemoveEvents removeEvents{m.destIndex, TrackIndex(idx)};
                trackStateHandler->applyRemoveEvents(removeEvents, activeState);

                t2.used = t1.used;
                t2.name.assign(t1.name.data(), t1.name.size());
                t2.busType = t1.busType;
                t2.deviceIndex = t1.deviceIndex;
                t2.on = t1.on;
                t2.programChange = t1.programChange;
                t2.transmitProgramChangesEnabled = t1.transmitProgramChangesEnabled;
                t2.velocityRatio = t1.velocityRatio;

                auto it = t1.eventsHead;

                while (it)
                {
                    EventData *e = acquireEvent();
                    std::memcpy(e, it, sizeof(EventData));
                    e->sequenceIndex = m.destIndex;
                    e->prev = nullptr;
                    e->next = nullptr;
                    insertAcquiredEvent(t2, e);
                    it = it->next;
                }
            }

            seqLock1.release();

































































            seqLock2.release();
        },
        [&](const CopyTrack &m)
        {
            auto &trackLock1 =
                trackLocks[m.sourceSequenceIndex][m.sourceTrackIndex];
            auto &trackLock2 =
                trackLocks[m.destSequenceIndex][m.destTrackIndex];

            if (!trackLock1.try_acquire() || !trackLock2.try_acquire())
            {
                enqueue(m);
                return;
            }

            applyCopyTrack(m);

            trackLock1.release();
            trackLock2.release();
        },
        [&](const UndoSequence &m)
        {
            auto &seqLock1 = sequenceLocks[activeState.selectedSequenceIndex];
            auto &seqLock2 = sequenceLocks[UndoSequenceIndex];

            if (!seqLock1.try_acquire() || !seqLock2.try_acquire())
            {
                enqueue(m);
                return;
            }

            auto &seqs = activeState.sequences;

            std::swap(seqs[activeState.selectedSequenceIndex],
                      seqs[UndoSequenceIndex]);

            activeState.undoSequenceAvailable =
                !activeState.undoSequenceAvailable;

            seqLock1.release();
            seqLock2.release();
        },
        [&](const SetUndoSequenceAvailable &m)
        {
            activeState.undoSequenceAvailable = m.available;
        }};

    std::visit(visitor, msg);
}

void SequencerStateManager::insertAcquiredEvent(TrackState &track,
                                                EventData *e) const
{
    assert(e);

    e->prev = nullptr;
    e->next = nullptr;

    EventData *&head = track.eventsHead;

    if (!head)
    {
        head = e;
        return;
    }

    int insertIndex = 0;

    if (e->tick < head->tick)
    {
        if (e->type == EventType::TempoChange &&
            head->type == EventType::TempoChange && head->amount == e->amount)
        {
            returnEventToPool(e);
            return;
        }

        e->next = head;
        head->prev = e;
        head = e;

        if (track.playEventIndex >= 0)
        {
            ++track.playEventIndex;
        }

        return;
    }

    EventData *it = head;
    int currentIndex = 0;

    while (it->next && it->next->tick <= e->tick)
    {
        it = it->next;
        currentIndex++;
    }

    insertIndex = currentIndex + 1;

    if (e->type == EventType::TempoChange &&
        it->type == EventType::TempoChange && it->amount == e->amount)
    {
        returnEventToPool(e);
        return;
    }

    EventData *n = it->next;

    it->next = e;
    e->prev = it;

    e->next = n;

    if (n)
    {
        n->prev = e;
    }

    if (track.playEventIndex >= insertIndex)
    {
        ++track.playEventIndex;
    }
}

EventData *SequencerStateManager::findRecordingNoteOnEvent(
    const SequenceIndex sequenceIndex, const TrackIndex trackIndex,
    const NoteNumber noteNumber)
{
    EventData *found = nullptr;

    const size_t liveNoteOnCount = liveNoteOnEventRecordingQueue.dequeue_bulk(
        tempLiveNoteOnRecordingEvents);

    for (size_t i = 0; i < liveNoteOnCount; ++i)
    {
        if (const auto e = tempLiveNoteOnRecordingEvents[i];
            e->noteNumber == noteNumber)
        {
            found = e;
            break;
        }
    }

    for (size_t i = 0; i < liveNoteOnCount; ++i)
    {
        EventData *e = tempLiveNoteOnRecordingEvents[i];
        liveNoteOnEventRecordingQueue.enqueue(e);
    }

    tempLiveNoteOnRecordingEvents.clear();
    tempLiveNoteOnRecordingEvents.resize(LIVE_NOTE_EVENT_RECORDING_CAPACITY);

    if (!found)
    {
        const TrackStateView trackStateView(
            activeState.sequences[sequenceIndex].tracks[trackIndex]);
        found = trackStateView.findRecordingNoteOnByNoteNumber(noteNumber);
    }

    return found;
}

mpc::Tick
SequencerStateManager::getCorrectedTickPos(const Tick currentPositionTicks,
                                           const SequenceStateView &seq) const
{
    const auto pos = currentPositionTicks;
    auto correctedTickPos = NoTick;

    const auto timingCorrectScreen =
        sequencer->getScreens()->get<lcdgui::ScreenId::TimingCorrectScreen>();

    const auto swingPercentage = timingCorrectScreen->getSwing();
    const auto noteValueLengthInTicks =
        timingCorrectScreen->getNoteValueLengthInTicks();

    if (noteValueLengthInTicks > 1)
    {
        correctedTickPos =
            Track::timingCorrectTick(seq, 0, seq.getLastBarIndex(), pos,
                                     noteValueLengthInTicks, swingPercentage);
    }

    if (timingCorrectScreen->getAmount() != 0)
    {
        auto shiftedTick = correctedTickPos != NoTick ? correctedTickPos : pos;
        auto amount = timingCorrectScreen->getAmount();

        if (!timingCorrectScreen->isShiftTimingLater())
        {
            amount *= -1;
        }

        shiftedTick += amount;

        if (shiftedTick < 0)
        {
            shiftedTick = 0;
        }

        if (const auto lastTick = seq.getLastTick(); shiftedTick > lastTick)
        {
            shiftedTick = lastTick;
        }

        correctedTickPos = shiftedTick;
    }

    return correctedTickPos;
}

void SequencerStateManager::applyCopyTrack(const CopyTrack &m)
{
    const auto &t1 =
        activeState.sequences[m.sourceSequenceIndex].tracks[m.sourceTrackIndex];
    auto &t2 =
        activeState.sequences[m.destSequenceIndex].tracks[m.destTrackIndex];

    t2.used = t1.used;
    t2.name.assign(t1.name.data(), t1.name.size());
    t2.busType = t1.busType;
    t2.deviceIndex = t1.deviceIndex;
    t2.on = t1.on;
    t2.programChange = t1.programChange;
    t2.transmitProgramChangesEnabled = t1.transmitProgramChangesEnabled;
    t2.velocityRatio = t1.velocityRatio;

    applyMessageImmediate(RemoveEvents{m.destSequenceIndex, m.destTrackIndex});

    const EventData *it = t1.eventsHead;

    while (it)
    {
        EventData *e = acquireEvent();
        std::memcpy(e, it, sizeof(EventData));
        e->sequenceIndex = m.destSequenceIndex;
        e->trackIndex = m.destTrackIndex;
        e->prev = nullptr;
        e->next = nullptr;
        insertAcquiredEvent(t2, e);
        it = it->next;
    }
}
