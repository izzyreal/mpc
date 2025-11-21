#include "sequencer/ControlChangeEvent.hpp"

using namespace mpc::sequencer;

ControlChangeEvent::ControlChangeEvent(
    const std::function<sequencer::EventState()> &getSnapshot)
    : Event(getSnapshot)
{
}

ControlChangeEvent::ControlChangeEvent(const ControlChangeEvent &event)
    : Event(event)
{
    setAmount(event.getAmount());
    setController(event.getController());
}

void ControlChangeEvent::setController(const int i)
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

void ControlChangeEvent::setAmount(const int i)
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
