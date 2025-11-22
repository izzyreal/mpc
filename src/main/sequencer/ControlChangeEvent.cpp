#include "sequencer/ControlChangeEvent.hpp"

using namespace mpc::sequencer;

ControlChangeEvent::ControlChangeEvent(
    const std::function<std::pair<EventIndex, EventState>()> &getSnapshot,
    const std::function<void(TrackEventMessage &&)> &dispatch)
    : Event(getSnapshot, dispatch)
{
}

void ControlChangeEvent::setController(const int i) const
{
    auto e = getSnapshot();
    e.second.controllerNumber = i;
    dispatch(UpdateEvent{e});
}

int ControlChangeEvent::getController() const
{
    return getSnapshot().second.controllerNumber;
}

void ControlChangeEvent::setAmount(const int i) const
{
    auto e = getSnapshot();
    e.second.controllerValue = i;
    dispatch(UpdateEvent{e});
}

int ControlChangeEvent::getAmount() const
{
    return getSnapshot().second.controllerValue;
}
