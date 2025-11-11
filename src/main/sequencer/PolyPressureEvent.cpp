#include "sequencer/PolyPressureEvent.hpp"

using namespace mpc::sequencer;

void PolyPressureEvent::setNote(const int i)
{
    if (i < 0 || i > 127)
    {
        return;
    }
    note = i;
}

int PolyPressureEvent::getNote() const
{
    return note;
}

void PolyPressureEvent::setAmount(const int i)
{
    if (i < 0 || i > 127)
    {
        return;
    }

    polyPressureValue = i;
}

int PolyPressureEvent::getAmount() const
{
    return polyPressureValue;
}

mpc::sequencer::PolyPressureEvent::PolyPressureEvent(
    const PolyPressureEvent &event)
    : Event(event)
{
    setAmount(event.getAmount());
    setNote(event.getNote());
}
