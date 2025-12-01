#include "sequencer/ControlChangeEvent.hpp"

using namespace mpc::sequencer;

ControlChangeEvent::ControlChangeEvent(
    EventData *const ptr, const EventData &snapshot,
    const std::function<void(TrackMessage &&)> &dispatch)
    : EventRef(ptr, snapshot, dispatch)
{
}

void ControlChangeEvent::setController(const int i) const
{
    auto e = snapshot;
    e.controllerNumber = i;
    dispatch(UpdateEvent{handle, e});
}

int ControlChangeEvent::getController() const
{
    return snapshot.controllerNumber;
}

void ControlChangeEvent::setAmount(const int i) const
{
    auto e = snapshot;
    e.controllerValue = i;
    dispatch(UpdateEvent{handle, e});
}

int ControlChangeEvent::getAmount() const
{
    return snapshot.controllerValue;
}
