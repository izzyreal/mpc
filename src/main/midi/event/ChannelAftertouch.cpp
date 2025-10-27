#include "midi/event/ChannelAftertouch.hpp"

#include "midi/event/ChannelEvent.hpp"

using namespace mpc::midi::event;

ChannelAftertouch::ChannelAftertouch(int tick, int channel, int amount)
    : ChannelEvent(tick, ChannelEvent::CHANNEL_AFTERTOUCH, channel, amount, 0)
{
}

ChannelAftertouch::ChannelAftertouch(int tick, int delta, int channel,
                                     int amount)
    : ChannelEvent(tick, delta, ChannelEvent::CHANNEL_AFTERTOUCH, channel,
                   amount, 0)
{
}

int ChannelAftertouch::getAmount()
{
    return mValue1;
}

void ChannelAftertouch::setAmount(int p)
{
    mValue1 = p;
}
