#include "sequencer/Track.hpp"

#include "sequencer/Sequence.hpp"
#include "sequencer/SequencerStateManager.hpp"
#include "sequencer/TrackStateView.hpp"

#include <concurrentqueue.h>

using namespace mpc::sequencer;

void Track::assignTicksToNoteOffs(const int noteOffCount, const Tick pos)
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

void Track::correctNoteOnTick(EventData *noteOn, const Tick pos,
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

void Track::adjustNoteOffTiming(const EventData *noteOn,
                                EventData &noteOff) const
{
    auto newTick = noteOff.tick + noteOn->wasMoved;
    const auto lastTick = parent->getLastTick();

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

bool Track::applyNoteOffAdjustmentForNoteOn(EventData *noteOn,
                                            const int noteOffCount)
{
    for (int i = 0; i < noteOffCount; ++i)
    {
        auto &noteOff = tempLiveNoteOffRecordingEvents[i];
        if (noteOff.noteNumber != noteOn->noteNumber)
        {
            continue;
        }

        adjustNoteOffTiming(noteOn, noteOff);
        finalizeRecordedNote(noteOn, noteOff);
        return true;
    }

    return false;
}

void Track::finalizeRecordedNote(EventData *noteOn, const EventData &noteOff)
{
    const auto lastTick = parent->getLastTick();
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

    const bool insert =
        getSnapshot(getIndex())
            ->findNoteEvent(noteOn->tick, noteOn->noteNumber) == nullptr;

    if (insert)
    {
        insertAcquiredEvent(noteOn);
    }
    else
    {
        manager->returnEventToPool(noteOn);
    }
}

void Track::processLiveNoteEventRecordingQueues()
{
    const int noteOnCount = liveNoteOnEventRecordingQueue->try_dequeue_bulk(
        tempLiveNoteOnRecordingEvents.begin(), 20);

    const int noteOffCount = liveNoteOffEventRecordingQueue->try_dequeue_bulk(
        tempLiveNoteOffRecordingEvents.begin(), 20);

    if (!noteOnCount && !noteOffCount)
    {
        return;
    }

    const Tick pos = getTickPosition();
    const Tick correctedPos = getCorrectedTickPos();

    assignTicksToNoteOffs(noteOffCount, pos);

    for (int i = 0; i < noteOnCount; ++i)
    {
        auto noteOn = tempLiveNoteOnRecordingEvents[i];

        correctNoteOnTick(noteOn, pos, correctedPos);

        const bool matched =
            applyNoteOffAdjustmentForNoteOn(noteOn, noteOffCount);

        if (!matched)
        {
            liveNoteOnEventRecordingQueue->enqueue(noteOn);
        }
    }
}

EventData *Track::recordNoteEventLive(const NoteNumber note,
                                      const Velocity velocity) const
{
    EventData *e = manager->acquireEvent();
    e->type = EventType::NoteOn;
    e->noteNumber = note;
    e->velocity = velocity;
    e->beingRecorded = true;
    e->sequenceIndex = parent->getSequenceIndex();
    e->trackIndex = trackIndex;
    e->tick = NoTickAssignedWhileRecording;
    liveNoteOnEventRecordingQueue->enqueue(e);
    return e;
}

void Track::finalizeNoteEventLive(const EventData *e,
                                  const Tick noteOffPositionTicks) const
{
    EventData noteOff;
    noteOff.type = EventType::NoteOff;
    noteOff.noteNumber = e->noteNumber;
    noteOff.tick = noteOffPositionTicks;
    liveNoteOffEventRecordingQueue->enqueue(noteOff);
}

void Track::flushNoteCache() const
{
    EventData *e;
    while (liveNoteOnEventRecordingQueue->try_dequeue(e))
    {
    }
    while (liveNoteOffEventRecordingQueue->try_dequeue(*e))
    {
    }
}
