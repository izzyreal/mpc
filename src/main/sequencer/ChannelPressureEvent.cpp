#include "sequencer/ChannelPressureEvent.hpp"

using namespace mpc::sequencer;

ChannelPressureEvent::ChannelPressureEvent(
    const std::function<std::pair<EventIndex, EventState>()> &getSnapshot,
    const std::function<void(TrackEventMessage &&)> &dispatch)
    : Event(getSnapshot, dispatch)
{
}

void ChannelPressureEvent::setAmount(const int i) const
{
    auto e = getSnapshot();
    e.second.amount = i;
    dispatch(UpdateEvent{e});
}

int ChannelPressureEvent::getAmount() const
{
    return getSnapshot().second.amount;
}
