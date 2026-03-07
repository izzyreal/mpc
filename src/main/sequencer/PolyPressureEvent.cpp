#include "sequencer/PolyPressureEvent.hpp"

using namespace mpc::sequencer;

PolyPressureEvent::PolyPressureEvent(
    EventData *const ptr, const EventData &snapshot,
    const std::function<void(TrackMessage &&)> &dispatch)
    : EventRef(ptr, snapshot, dispatch)
{
}

void PolyPressureEvent::setNote(const int i) const
{
    auto e = snapshot;
    e.noteNumber = NoteNumber(static_cast<int8_t>(std::clamp(
        i, static_cast<int>(MinNoteNumber), static_cast<int>(MaxNoteNumber))));
    dispatch(UpdateEvent{handle, e});
}

int PolyPressureEvent::getNote() const
{
    return snapshot.noteNumber;
}

void PolyPressureEvent::setAmount(const int i) const
{
    auto e = snapshot;
    e.amount = static_cast<uint8_t>(std::clamp(i, 0, 127));
    dispatch(UpdateEvent{handle, e});
}

int PolyPressureEvent::getAmount() const
{
    return snapshot.amount;
}
