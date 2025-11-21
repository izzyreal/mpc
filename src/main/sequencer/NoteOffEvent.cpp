#include "sequencer/NoteOffEvent.hpp"

using namespace mpc::sequencer;

NoteOffEvent::NoteOffEvent(
            const std::function<EventState()> &getSnapshot,
            const NoteNumber noteNumber)
            : Event(getSnapshot)
{
    setNote(noteNumber);
}

NoteOffEvent::NoteOffEvent(
            const std::function<EventState()> &getSnapshot)
            : Event(getSnapshot)
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
