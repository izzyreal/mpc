#include "sequencer/PolyPressureEvent.hpp"

using namespace mpc::sequencer;

PolyPressureEvent::PolyPressureEvent(
    EventState *eventState,
    const std::function<void(SequenceMessage &&)> &dispatch)
    : Event(eventState, dispatch)
{
}

void PolyPressureEvent::setNote(const int i) const
{
    auto e = *eventState;
    e.noteNumber = NoteNumber(i);
    dispatch(UpdateEvent{eventState, e});
}

int PolyPressureEvent::getNote() const
{
    return eventState->noteNumber;
}

void PolyPressureEvent::setAmount(const int i) const
{
    auto e = *eventState;
    e.amount = i;
    dispatch(UpdateEvent{eventState, e});
}

int PolyPressureEvent::getAmount() const
{
    return eventState->amount;
}
