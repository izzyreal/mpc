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

    liveNoteOnEventRecordingQueue = moodycamel::ConcurrentQueue<EventData *>(
        LIVE_NOTE_EVENT_RECORDING_CAPACITY);
    liveNoteOffEventRecordingQueue = moodycamel::ConcurrentQueue<EventData>(
        LIVE_NOTE_EVENT_RECORDING_CAPACITY);
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
            trackStateHandler->applyMessage(activeState, actions, m);
        },
        [&](const SongMessage &m)
        {
            songStateHandler->applyMessage(activeState, actions, m);
        },
        [&](const SwitchToNextSequence &m)
        {
            constexpr bool setPositionTo0 = false;
            enqueue(SetSelectedSequenceIndex{m.sequenceIndex, setPositionTo0});
        },
        [&](const SetSelectedSequenceIndex &m)
        {
            activeState.selectedSequenceIndex = m.sequenceIndex;

            if (m.setPositionTo0)
            {
                activeState.transport.positionQuarterNotes = 0;
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
            head->type == EventType::TempoChange &&
            head->amount == e->amount)
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
        it->type == EventType::TempoChange &&
        it->amount == e->amount)
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

    const auto count = liveNoteOnEventRecordingQueue.try_dequeue_bulk(
        tempLiveNoteOnRecordingEvents.begin(),
        tempLiveNoteOnRecordingEvents.size());

    for (int i = 0; i < count; i++)
    {
        const auto e = tempLiveNoteOnRecordingEvents[i];
        if (e->noteNumber == noteNumber)
        {
            found = e;
            break;
        }
    }

    for (size_t i = 0; i < count; i++)
    {
        liveNoteOnEventRecordingQueue.enqueue(tempLiveNoteOnRecordingEvents[i]);
    }

    tempLiveNoteOnRecordingEvents.clear();
    tempLiveNoteOnRecordingEvents.resize(20);

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
