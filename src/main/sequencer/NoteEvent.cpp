#include "sequencer/NoteEvent.hpp"

#include <algorithm>

using namespace mpc::sequencer;

bool mpc::sequencer::isDrumNote(const NoteNumber number)
{
    return number >= MinDrumNoteNumber && number <= MaxDrumNoteNumber;
}

void NoteOffEvent::setNote(const NoteNumber i)
{
    // number = i;
}

mpc::NoteNumber NoteOffEvent::getNote() const
{
    return getSnapshot().noteNumber;
}

NoteOnEvent::NoteOnEvent(
    const std::function<sequencer::EventState()> &getSnapshot,
    const NoteNumber noteNumber, const Velocity vel, const NoteEventId id)
    : Event(getSnapshot), id(id)
{
    noteOff = std::make_shared<NoteOffEvent>(getSnapshot);
    setNote(noteNumber);
    setVelocity(vel);
}

NoteOnEvent::NoteOnEvent(
    const std::function<sequencer::EventState()> &getSnapshot)
    : Event(getSnapshot), id(NoNoteEventId)
{
    noteOff = std::make_shared<NoteOffEvent>(getSnapshot);
}
NoteOnEvent::NoteOnEvent(
    const std::function<sequencer::EventState()> &getSnapshot,
    const DrumNoteNumber drumNoteNumber)
    : Event(getSnapshot), id(NoNoteEventId)
{
    noteOff = std::make_shared<NoteOffEvent>(getSnapshot);
    setNote(drumNoteNumber);
}

void NoteOnEvent::setMetronomeOnlyTickPosition(const int pos)
{
    metronomeOnlyTickPosition = pos;
}

int NoteOnEvent::getMetronomeOnlyTickPosition() const
{
    return metronomeOnlyTickPosition;
}

std::shared_ptr<NoteOffEvent> NoteOnEvent::getNoteOff() const
{
    return noteOff;
}

void NoteOnEvent::setTrack(const TrackIndex trackIndexToUse)
{
    track = trackIndexToUse;
    noteOff->setTrack(trackIndexToUse);
}
void NoteOnEvent::setBeingRecorded(bool b)
{
    beingRecorded = b;
}

bool NoteOnEvent::isBeingRecorded() const
{
    return beingRecorded;
}

void NoteOnEvent::setNote(const NoteNumber n)
{
    // noteNumber = n;
    noteOff->setNote(n);
}

mpc::NoteNumber NoteOnEvent::getNote() const
{
    return getSnapshot().noteNumber;
}

void NoteOnEvent::setDuration(const Duration d)
{
    if (d)
    {
        // duration = std::clamp(*d, 0, 9999);
    }
}

mpc::Duration NoteOnEvent::getDuration() const
{
    return getSnapshot().duration;
}

void NoteOnEvent::resetDuration()
{
    setDuration(NoDuration);
}

mpc::NoteVariationType NoteOnEvent::getVariationType() const
{
    return getSnapshot().noteVariationType;
}

void NoteOnEvent::incrementVariationType(const int amount)
{
    // variationType = static_cast<VARIATION_TYPE>(
    //     std::clamp(static_cast<int>(variationType) + amount, 0, 3));
}

void NoteOnEvent::setVariationType(const NoteVariationType type)
{
    // variationType = type;
}

void NoteOnEvent::setVariationValue(const int i)
{
    // if (variationType == TUNE_0)
    // {
    //     variationValue = std::clamp(i, 0, 124);
    // }
    // else
    // {
    //     variationValue = std::clamp(i, 0, 100);
    // }
}

int NoteOnEvent::getVariationValue() const
{
    return getSnapshot().noteVariationValue;
}

void NoteOnEvent::setVelocity(const Velocity i)
{
    // velocity = i;
}

mpc::Velocity NoteOnEvent::getVelocity() const
{
    return getSnapshot().velocity;
}

bool NoteOnEvent::isFinalized() const
{
    return getDuration() != NoDuration;
}

uint32_t NoteOnEvent::getId() const
{
    return id;
}
