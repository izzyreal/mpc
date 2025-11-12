#include "sequencer/ChannelPressureEvent.hpp"

using namespace mpc::sequencer;

void ChannelPressureEvent::setAmount(const int i)
{
    if (i < 0 || i > 127)
    {
        return;
    }
    channelPressureValue = i;
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
