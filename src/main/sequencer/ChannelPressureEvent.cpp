#include "sequencer/ChannelPressureEvent.hpp"

using namespace mpc::sequencer;

void ChannelPressureEvent::setAmount(int i)
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

mpc::sequencer::ChannelPressureEvent::ChannelPressureEvent(
    const ChannelPressureEvent &event)
    : Event(event)
{
    setAmount(event.getAmount());
}
