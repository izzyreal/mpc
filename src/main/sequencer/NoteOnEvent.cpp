#include "sequencer/NoteOnEvent.hpp"

using namespace mpc::sequencer;

NoteOnEvent::NoteOnEvent(
    const std::function<EventState()> &getSnapshot,
    const std::function<void(TrackEventMessage &&)> &dispatch,
    const NoteNumber noteNumber, const Velocity vel)
    : Event(getSnapshot, dispatch)
{
    setNote(noteNumber);
    setVelocity(vel);
}

NoteOnEvent::NoteOnEvent(
    const std::function<EventState()> &getSnapshot,
    const std::function<void(TrackEventMessage &&)> &dispatch)
    : Event(getSnapshot, dispatch)
{
}

NoteOnEvent::NoteOnEvent(
    const std::function<EventState()> &getSnapshot,
    const std::function<void(TrackEventMessage &&)> &dispatch,
    const DrumNoteNumber drumNoteNumber)
    : Event(getSnapshot, dispatch)
{
    setNote(drumNoteNumber);
}

void NoteOnEvent::setNote(const NoteNumber n) const
{
    auto e = getSnapshot();
    e.noteNumber = n;
    dispatch(UpdateEvent{e});
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
