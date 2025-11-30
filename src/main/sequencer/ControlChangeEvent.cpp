#include "sequencer/ControlChangeEvent.hpp"

using namespace mpc::sequencer;

ControlChangeEvent::ControlChangeEvent(
    EventState *eventState,
    const std::function<void(TrackMessage &&)> &dispatch)
    : Event(eventState, dispatch)
{
}

void ControlChangeEvent::setController(const int i) const
{
    auto e = *eventState;
    e.controllerNumber = i;
    dispatch(UpdateEvent{eventState, e});
}

int ControlChangeEvent::getController() const
{
    return eventState->controllerNumber;
}

void ControlChangeEvent::setAmount(const int i) const
{
    auto e = *eventState;
    e.controllerValue = i;
    dispatch(UpdateEvent{eventState, e});
}

int ControlChangeEvent::getAmount() const
{
    return eventState->controllerValue;
}
