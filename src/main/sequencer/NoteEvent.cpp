#include "sequencer/NoteEvent.hpp"

#include <algorithm>

using namespace mpc::sequencer;

bool mpc::sequencer::isDrumNote(const NoteNumber number)
{
    return number >= 35 && number <= 98;
}

void NoteOffEvent::setNote(const NoteNumber i)
{
    number = i;
}

mpc::NoteNumber NoteOffEvent::getNote() const
{
    return number;
}

NoteOnEvent::NoteOnEvent(const NoteNumber noteNumber, const Velocity vel,
                         const NoteEventId id)
    : id(id), noteNumber(noteNumber)
{
    noteOff = std::shared_ptr<NoteOffEvent>(new NoteOffEvent());
    setNote(noteNumber);
    setVelocity(vel);
}

NoteOnEvent::NoteOnEvent() : id(NoNoteEventId)
{
    noteOff = std::shared_ptr<NoteOffEvent>(new NoteOffEvent());
}
NoteOnEvent::NoteOnEvent(const DrumNoteNumber drumNoteNumber)
    : id(NoNoteEventId), noteNumber(drumNoteNumber)
{
    noteOff = std::shared_ptr<NoteOffEvent>(new NoteOffEvent());
}

NoteOnEvent::NoteOnEvent(const NoteOnEvent &event) : Event(event)
{
    noteOff = std::shared_ptr<NoteOffEvent>(new NoteOffEvent());
    setNote(event.noteNumber);
    setVelocity(event.velocity);
    setDuration(event.duration);
    setVariationType(event.variationType);
    setVariationValue(event.variationValue);
    NoteOnEvent::setTrack(event.track);
}

bool NoteOnEvent::finalizeNonLive(const int newDuration)
{
    const auto oldDuration = getDuration();
    setDuration(newDuration);
    setBeingRecorded(false);
    return oldDuration != duration;
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
    noteNumber = n;
    noteOff->setNote(n);
}

mpc::NoteNumber NoteOnEvent::getNote() const
{
    return noteNumber;
}

void NoteOnEvent::setDuration(const Duration d)
{
    if (d)
    {
        duration = std::clamp(*d, 0, 9999);
    }
}

NoteOnEvent::Duration NoteOnEvent::getDuration() const
{
    return duration;
}

void NoteOnEvent::resetDuration()
{
    duration = std::nullopt;
}

NoteOnEvent::VARIATION_TYPE NoteOnEvent::getVariationType() const
{
    return variationType;
}

void NoteOnEvent::incrementVariationType(const int amount)
{
    variationType = static_cast<VARIATION_TYPE>(
        std::clamp(static_cast<int>(variationType) + amount, 0, 3));
}

void NoteOnEvent::setVariationType(const VARIATION_TYPE type)
{
    variationType = type;
}

void NoteOnEvent::setVariationValue(const int i)
{
    if (variationType == TUNE_0)
    {
        variationValue = std::clamp(i, 0, 124);
    }
    else
    {
        variationValue = std::clamp(i, 0, 100);
    }
}

int NoteOnEvent::getVariationValue() const
{
    return variationValue;
}

void NoteOnEvent::setVelocity(const Velocity i)
{
    velocity = i;
}

mpc::Velocity NoteOnEvent::getVelocity() const
{
    return velocity;
}

bool NoteOnEvent::isFinalized() const
{
    return duration.has_value();
}

bool NoteOnEvent::isPlayOnly()
{
    return dynamic_cast<NoteOnEventPlayOnly *>(this) != nullptr;
}

uint32_t NoteOnEvent::getId() const
{
    return id;
}
