#include "sequencer/ControlChangeEvent.hpp"

using namespace mpc::sequencer;

ControlChangeEvent::ControlChangeEvent(
    const std::function<EventState()> &getSnapshot,
    const std::function<void(SequencerMessage &&)> &dispatch)
    : Event(getSnapshot, dispatch)
{
}

void ControlChangeEvent::setController(const int i) const
{
    auto e = getSnapshot();
    e.controllerNumber = i;
    dispatch(UpdateEvent{e});
}

int ControlChangeEvent::getController() const
{
    return getSnapshot().controllerNumber;
}

void ControlChangeEvent::setAmount(const int i) const
{
    auto e = getSnapshot();
    e.controllerValue = i;
    dispatch(UpdateEvent{e});
}

int ControlChangeEvent::getAmount() const
{
    return getSnapshot().controllerValue;
}
