#include "sequencer/ChannelPressureEvent.hpp"

using namespace mpc::sequencer;

ChannelPressureEvent::ChannelPressureEvent(
    const std::function<sequencer::EventState()> &getSnapshot)
        : Event(getSnapshot)
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
