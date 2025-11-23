#include "sequencer/PolyPressureEvent.hpp"

using namespace mpc::sequencer;

PolyPressureEvent::PolyPressureEvent(
    const std::function<std::pair<EventId, EventState>()> &getSnapshot,
    const std::function<void(TrackEventMessage &&)> &dispatch)
    : Event(getSnapshot, dispatch)
{
}

void PolyPressureEvent::setNote(const int i) const
{
    auto e = getSnapshot();
    e.second.noteNumber = NoteNumber(i);
    dispatch(UpdateEvent{e});
}

int PolyPressureEvent::getNote() const
{
    return getSnapshot().second.noteNumber;
}

void PolyPressureEvent::setAmount(const int i) const
{
    auto e = getSnapshot();
    e.second.amount = i;
    dispatch(UpdateEvent{e});
}

int PolyPressureEvent::getAmount() const
{
    return getSnapshot().second.amount;
}
