#include "sequencer/PolyPressureEvent.hpp"

using namespace mpc::sequencer;

PolyPressureEvent::PolyPressureEvent(
    const std::function<EventState()> &getSnapshot,
    const std::function<void(TrackEventMessage &&)> &dispatch)
    : Event(getSnapshot, dispatch)
{
}

void PolyPressureEvent::setNote(const int i) const
{
    auto e = getSnapshot();
    e.noteNumber = NoteNumber(i);
    dispatch(UpdateEvent{e});
}

int PolyPressureEvent::getNote() const
{
    return getSnapshot().noteNumber;
}

void PolyPressureEvent::setAmount(const int i) const
{
    auto e = getSnapshot();
    e.amount = i;
    dispatch(UpdateEvent{e});
}

int PolyPressureEvent::getAmount() const
{
    return getSnapshot().amount;
}
