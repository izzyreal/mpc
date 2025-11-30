#include "sequencer/ChannelPressureEvent.hpp"

using namespace mpc::sequencer;

ChannelPressureEvent::ChannelPressureEvent(
    EventState *eventState,
    const std::function<void(TrackMessage &&)> &dispatch)
    : Event(eventState, dispatch)
{
}

void ChannelPressureEvent::setAmount(const int i) const
{
    auto e = *eventState;
    e.amount = i;
    dispatch(UpdateEvent{eventState, e});
}

int ChannelPressureEvent::getAmount() const
{
    return eventState->amount;
}
