#include "sequencer/NoteOnEvent.hpp"

#include "sequencer/NoteOffEvent.hpp"

using namespace mpc::sequencer;

NoteOnEvent::NoteOnEvent(
    const std::function<EventState()> &getSnapshot,
    const std::function<void(TrackEventMessage &&)> &dispatch,
    const NoteNumber noteNumber, const Velocity vel, const NoteEventId id)
    : Event(getSnapshot, dispatch), id(id)
{
    noteOff = std::make_shared<NoteOffEvent>(getSnapshot, dispatch);
    setNote(noteNumber);
    setVelocity(vel);
}

NoteOnEvent::NoteOnEvent(
    const std::function<EventState()> &getSnapshot, const std::function<void(TrackEventMessage &&)> &dispatch)
    : Event(getSnapshot, dispatch), id(NoNoteEventId)
{
    noteOff = std::make_shared<NoteOffEvent>(getSnapshot, dispatch);
}

NoteOnEvent::NoteOnEvent(
    const std::function<EventState()> &getSnapshot,
    const std::function<void(TrackEventMessage &&)> &dispatch,
    const DrumNoteNumber drumNoteNumber)
    : Event(getSnapshot, dispatch), id(NoNoteEventId)
{
    noteOff = std::make_shared<NoteOffEvent>(getSnapshot, dispatch);
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
void NoteOnEvent::setBeingRecorded(const bool b)
{
    beingRecorded = b;
}

bool NoteOnEvent::isBeingRecorded() const
{
    return beingRecorded;
}

void NoteOnEvent::setNote(const NoteNumber n) const
{
    auto e = getSnapshot();
    e.noteNumber = n;
    dispatch(UpdateEvent{e});
    noteOff->setNote(n);
}

mpc::NoteNumber NoteOnEvent::getNote() const
{
    return getSnapshot().noteNumber;
}

void NoteOnEvent::setDuration(const Duration duration) const
{
    auto e = getSnapshot();
    e.duration = duration;
    dispatch(UpdateEvent{e});
}

mpc::Duration NoteOnEvent::getDuration() const
{
    return getSnapshot().duration;
}

void NoteOnEvent::resetDuration() const
{
    setDuration(NoDuration);
}

mpc::NoteVariationType NoteOnEvent::getVariationType() const
{
    return getSnapshot().noteVariationType;
}

void NoteOnEvent::setVariationType(const NoteVariationType type) const
{
    auto e = getSnapshot();
    e.noteVariationType = type;
    dispatch(UpdateEvent{e});
}

void NoteOnEvent::setVariationValue(const int i) const
{
    auto e = getSnapshot();
    e.noteVariationValue = NoteVariationValue(i);
    dispatch(UpdateEvent{e});
}

int NoteOnEvent::getVariationValue() const
{
    return getSnapshot().noteVariationValue;
}

void NoteOnEvent::setVelocity(const Velocity velocity) const
{
    auto e = getSnapshot();
    e.velocity = velocity;
    dispatch(UpdateEvent{e});
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
