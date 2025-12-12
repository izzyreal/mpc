#include "SequenceStateView.hpp"
#include "sequencer/SequencerStateManager.hpp"

#include "sequencer/Sequence.hpp"
#include "sequencer/TrackStateView.hpp"

using namespace mpc::sequencer;

void SequencerStateManager::assignTicksToNoteOffs(const int noteOffCount,
                                                  const Tick pos)
{
    for (int i = 0; i < noteOffCount; ++i)
    {
        auto &noteOff = tempLiveNoteOffRecordingEvents[i];
        if (noteOff.tick == NoTickAssignedWhileRecording)
        {
            noteOff.tick = pos;
        }
    }
}

void SequencerStateManager::correctNoteOnTick(EventData *noteOn, const Tick pos,
                                              const Tick correctedPos)
{
    if (noteOn->tick != NoTickAssignedWhileRecording)
    {
        return;
    }

    noteOn->tick =
        correctedPos != pos && correctedPos != NoTick ? correctedPos : pos;

    noteOn->wasMoved = noteOn->tick - pos;
}

void SequencerStateManager::adjustNoteOffTiming(const EventData *noteOn,
                                                EventData &noteOff,
                                                const Tick lastTick) const
{
    auto newTick = noteOff.tick + noteOn->wasMoved;

    if (newTick < 0)
    {
        newTick += lastTick;
    }
    else if (newTick > lastTick)
    {
        newTick -= lastTick;
    }

    noteOff.tick = newTick;
}

bool SequencerStateManager::applyNoteOffAdjustmentForNoteOn(
    EventData *noteOn, const int noteOffCount, const Tick lastTick)
{
    for (int i = 0; i < noteOffCount; ++i)
    {
        auto &noteOff = tempLiveNoteOffRecordingEvents[i];

        if (noteOff.noteNumber != noteOn->noteNumber)
        {
            continue;
        }

        adjustNoteOffTiming(noteOn, noteOff, lastTick);
        finalizeRecordedNote(noteOn, noteOff, lastTick);
        return true;
    }

    return false;
}

void SequencerStateManager::finalizeRecordedNote(EventData *noteOn,
                                                 const EventData &noteOff,
                                                 const Tick lastTick)
{
    auto duration = noteOff.tick - noteOn->tick;

    if (noteOff.tick < noteOn->tick)
    {
        duration = lastTick - noteOn->tick;
        noteOn->dontDelete = true;
    }

    if (duration < 1)
    {
        duration = 1;
    }
    noteOn->duration = Duration(duration);
    noteOn->beingRecorded = false;

    auto &track =
        activeState.sequences[noteOn->sequenceIndex].tracks[noteOn->trackIndex];
    const TrackStateView trackStateView(track);

    const bool insert = trackStateView.findNoteEvent(
                            noteOn->tick, noteOn->noteNumber) == nullptr;

    if (insert)
    {
        insertAcquiredEvent(track, noteOn);
    }
    else
    {
        returnEventToPool(noteOn);
    }
}

void SequencerStateManager::processLiveNoteEventRecordingQueues(
    const Tick currentPositionTicks,
    const SequenceStateView& seq)
{
    int noteOnCount = 0;
    for (; noteOnCount < LIVE_NOTE_EVENT_RECORDING_CAPACITY; ++noteOnCount)
    {
        EventData* ptr;
        if (!liveNoteOnEventRecordingQueue.dequeue(ptr))
            break;

        tempLiveNoteOnRecordingEvents[noteOnCount] = ptr;
    }

    int noteOffCount = 0;
    for (; noteOffCount < LIVE_NOTE_EVENT_RECORDING_CAPACITY; ++noteOffCount)
    {
        alignas(EventData) unsigned char buf[sizeof(EventData)];
        const auto ev = reinterpret_cast<EventData*>(buf);

        if (!liveNoteOffEventRecordingQueue.dequeue(*ev))
            break;

        tempLiveNoteOffRecordingEvents[noteOffCount] = *ev;
        ev->~EventData();
    }

    if (!noteOnCount && !noteOffCount)
        return;

    const Tick pos = currentPositionTicks;
    const Tick correctedPos = getCorrectedTickPos(currentPositionTicks, seq);

    assignTicksToNoteOffs(noteOffCount, pos);

    for (int i = 0; i < noteOnCount; ++i)
    {
        EventData* noteOn = tempLiveNoteOnRecordingEvents[i];

        correctNoteOnTick(noteOn, pos, correctedPos);

        const bool matched =
            applyNoteOffAdjustmentForNoteOn(
                noteOn,
                noteOffCount,
                seq.getLastTick());

        if (!matched)
            liveNoteOnEventRecordingQueue.enqueue(noteOn);
    }
}

EventData *SequencerStateManager::recordNoteEventLive(
    const SequenceIndex sequenceIndex, const TrackIndex trackIndex,
    const NoteNumber note, const Velocity velocity)
{
    EventData *e = acquireEvent();
    e->type = EventType::NoteOn;
    e->noteNumber = note;
    e->velocity = velocity;
    e->beingRecorded = true;
    e->sequenceIndex = sequenceIndex;
    e->trackIndex = trackIndex;
    e->tick = NoTickAssignedWhileRecording;
    liveNoteOnEventRecordingQueue.enqueue(e);
    return e;
}

void SequencerStateManager::finalizeNoteEventLive(
    const EventData *e, const Tick noteOffPositionTicks)
{
    EventData noteOff;
    noteOff.type = EventType::NoteOff;
    noteOff.noteNumber = e->noteNumber;
    noteOff.tick = noteOffPositionTicks;
    liveNoteOffEventRecordingQueue.enqueue(noteOff);
}

void SequencerStateManager::flushNoteCache()
{
    EventData* ptr;
    while (liveNoteOnEventRecordingQueue.dequeue(ptr))
    {
    }

    alignas(EventData) unsigned char buf[sizeof(EventData)];
    const auto ev = reinterpret_cast<EventData*>(buf);

    while (liveNoteOffEventRecordingQueue.dequeue(*ev))
    {
        ev->~EventData();
    }
}