#include "sequencer/ChannelPressureEvent.hpp"

using namespace mpc::sequencer;

ChannelPressureEvent::ChannelPressureEvent(
    const std::function<EventState()> &getSnapshot,
    const std::function<void(TrackEventMessage &&)> &dispatch)
    : Event(getSnapshot, dispatch)
{
}

void ChannelPressureEvent::setAmount(const int i)
{
    channelPressureValue = std::clamp(i, 0, 127);
}

int ChannelPressureEvent::getAmount() const
{
    return channelPressureValue;
}

ChannelPressureEvent::ChannelPressureEvent(const ChannelPressureEvent &event)
    : Event(event)
{
    setAmount(event.getAmount());
}
