#include "sequencer/ChannelPressureEvent.hpp"

using namespace mpc::sequencer;

ChannelPressureEvent::ChannelPressureEvent(
    EventData *const ptr, const EventData &snapshot,
    const std::function<void(TrackMessage &&)> &dispatch)
    : EventRef(ptr, snapshot, dispatch)
{
}

void ChannelPressureEvent::setAmount(const int i) const
{
    auto e = snapshot;
    e.amount = i;
    dispatch(UpdateEvent{handle, e});
}

int ChannelPressureEvent::getAmount() const
{
    return snapshot.amount;
}
