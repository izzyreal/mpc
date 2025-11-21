#include "sequencer/NoteOffEvent.hpp"

using namespace mpc::sequencer;

NoteOffEvent::NoteOffEvent(
    const std::function<EventState()> &getSnapshot,
    const std::function<void(TrackEventMessage &&)> &dispatch,
    const NoteNumber noteNumber)
    : Event(getSnapshot, dispatch)
{
    setNote(noteNumber);
}

NoteOffEvent::NoteOffEvent(
    const std::function<EventState()> &getSnapshot,
    const std::function<void(TrackEventMessage &&)> &dispatch)
    : Event(getSnapshot, dispatch)
{
}

void NoteOffEvent::setNote(const NoteNumber i)
{
    // number = i;
}

mpc::NoteNumber NoteOffEvent::getNote() const
{
    return getSnapshot().noteNumber;
}
