#include "sequencer/ChannelPressureEvent.hpp"

using namespace mpc::sequencer;

ChannelPressureEvent::ChannelPressureEvent(
    const std::function<EventState()> &getSnapshot,
    const std::function<void(NonRtSequencerMessage &&)> &dispatch)
    : Event(getSnapshot, dispatch)
{
}

void ChannelPressureEvent::setAmount(const int i) const
{
    auto e = getSnapshot();
    e.amount = i;
    dispatch(UpdateEvent{e});
}

int ChannelPressureEvent::getAmount() const
{
    return getSnapshot().amount;
}
