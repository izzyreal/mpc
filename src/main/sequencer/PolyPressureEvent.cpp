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
    e.noteNumber = NoteNumber(i);
    dispatch(UpdateEvent{handle, e});
}

int PolyPressureEvent::getNote() const
{
    return snapshot.noteNumber;
}

void PolyPressureEvent::setAmount(const int i) const
{
    auto e = snapshot;
    e.amount = i;
    dispatch(UpdateEvent{handle, e});
}

int PolyPressureEvent::getAmount() const
{
    return snapshot.amount;
}
