#include "sequencer/NoteOnEvent.hpp"

using namespace mpc::sequencer;

NoteOnEvent::NoteOnEvent(
    EventData *const ptr, const EventData &snapshot,
    const std::function<void(TrackMessage &&)> &dispatch,
    const NoteNumber noteNumber, const Velocity vel)
    : EventRef(ptr, snapshot, dispatch)
{
    setNote(noteNumber);
    setVelocity(vel);
}

NoteOnEvent::NoteOnEvent(
    EventData *const ptr, const EventData &snapshot,
    const std::function<void(TrackMessage &&)> &dispatch)
    : EventRef(ptr, snapshot, dispatch)
{
}

NoteOnEvent::NoteOnEvent(
    EventData *const ptr, const EventData &snapshot,
    const std::function<void(TrackMessage &&)> &dispatch,
    const DrumNoteNumber drumNoteNumber)
    : EventRef(ptr, snapshot, dispatch)
{
    setNote(drumNoteNumber);
}

void NoteOnEvent::setNote(const NoteNumber n) const
{
    auto e = snapshot;
    e.noteNumber = n;
    dispatch(UpdateEvent{handle, e});
}

mpc::NoteNumber NoteOnEvent::getNote() const
{
    return snapshot.noteNumber;
}

void NoteOnEvent::setDuration(const Duration duration) const
{
    auto e = snapshot;
    e.duration = std::clamp(duration, MinDuration, MaxDuration);
    dispatch(UpdateEvent{handle, e});
}

mpc::Duration NoteOnEvent::getDuration() const
{
    return snapshot.duration;
}

void NoteOnEvent::resetDuration() const
{
    setDuration(NoDuration);
}

mpc::NoteVariationType NoteOnEvent::getVariationType() const
{
    return snapshot.noteVariationType;
}

void NoteOnEvent::setVariationType(const NoteVariationType type) const
{
    auto e = snapshot;
    e.noteVariationType = type;
    dispatch(UpdateEvent{handle, e});
}

void NoteOnEvent::setVariationValue(const int i) const
{
    auto e = snapshot;
    e.noteVariationValue = NoteVariationValue(i);
    dispatch(UpdateEvent{handle, e});
}

int NoteOnEvent::getVariationValue() const
{
    return snapshot.noteVariationValue;
}

void NoteOnEvent::setVelocity(const Velocity velocity) const
{
    auto e = snapshot;
    e.velocity = velocity;
    dispatch(UpdateEvent{handle, e});
}

mpc::Velocity NoteOnEvent::getVelocity() const
{
    return snapshot.velocity;
}
