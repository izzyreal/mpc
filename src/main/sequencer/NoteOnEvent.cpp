#include "sequencer/NoteOnEvent.hpp"

using namespace mpc::sequencer;

NoteOnEvent::NoteOnEvent(
    EventState *eventState,
    const std::function<void(TrackMessage &&)> &dispatch,
    const NoteNumber noteNumber, const Velocity vel)
    : Event(eventState, dispatch)
{
    setNote(noteNumber);
    setVelocity(vel);
}

NoteOnEvent::NoteOnEvent(
    EventState *eventState,
    const std::function<void(TrackMessage &&)> &dispatch)
    : Event(eventState, dispatch)
{
}

NoteOnEvent::NoteOnEvent(
    EventState *eventState,
    const std::function<void(TrackMessage &&)> &dispatch,
    const DrumNoteNumber drumNoteNumber)
    : Event(eventState, dispatch)
{
    setNote(drumNoteNumber);
}

void NoteOnEvent::setNote(const NoteNumber n) const
{
    auto e = *eventState;
    e.noteNumber = n;
    dispatch(UpdateEvent{eventState, e});
}

mpc::NoteNumber NoteOnEvent::getNote() const
{
    return eventState->noteNumber;
}

void NoteOnEvent::setDuration(const Duration duration) const
{
    auto e = *eventState;
    e.duration = std::clamp(duration, MinDuration, MaxDuration);
    dispatch(UpdateEvent{eventState, e});
}

mpc::Duration NoteOnEvent::getDuration() const
{
    return eventState->duration;
}

void NoteOnEvent::resetDuration() const
{
    setDuration(NoDuration);
}

mpc::NoteVariationType NoteOnEvent::getVariationType() const
{
    return eventState->noteVariationType;
}

void NoteOnEvent::setVariationType(const NoteVariationType type) const
{
    auto e = *eventState;
    e.noteVariationType = type;
    dispatch(UpdateEvent{eventState, e});
}

void NoteOnEvent::setVariationValue(const int i) const
{
    auto e = *eventState;
    e.noteVariationValue = NoteVariationValue(i);
    dispatch(UpdateEvent{eventState, e});
}

int NoteOnEvent::getVariationValue() const
{
    return eventState->noteVariationValue;
}

void NoteOnEvent::setVelocity(const Velocity velocity) const
{
    auto e = *eventState;
    e.velocity = velocity;
    dispatch(UpdateEvent{eventState, e});
}

mpc::Velocity NoteOnEvent::getVelocity() const
{
    return eventState->velocity;
}
