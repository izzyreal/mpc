#include "sequencer/ControlChangeEvent.hpp"

using namespace mpc::sequencer;

void ControlChangeEvent::setController(int i)
{
    if (i < 0 || i > 127)
    {
        return;
    }
    controllerNumber = i;
}

int ControlChangeEvent::getController() const
{
    return controllerNumber;
}

void ControlChangeEvent::setAmount(int i)
{
    if (i < 0 || i > 127)
    {
        return;
    }

    controllerValue = i;
}

int ControlChangeEvent::getAmount() const
{
    return controllerValue;
}

mpc::sequencer::ControlChangeEvent::ControlChangeEvent(
    const ControlChangeEvent &event)
    : Event(event)
{
    setAmount(event.getAmount());
    setController(event.getController());
}
