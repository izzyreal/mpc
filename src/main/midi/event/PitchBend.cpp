#include "midi/event/PitchBend.hpp"

#include "midi/event/ChannelEvent.hpp"

using namespace mpc::midi::event;

PitchBend::PitchBend(int tick, int channel, int lsb, int msb) : ChannelEvent(tick, ChannelEvent::PITCH_BEND, channel, lsb, msb)
{
}

PitchBend::PitchBend(int tick, int delta, int channel, int lsb, int msb) : ChannelEvent(tick, delta, ChannelEvent::PITCH_BEND, channel, lsb, msb)
{
}

int PitchBend::getLeastSignificantBits()
{
    return mValue1;
}

int PitchBend::getMostSignificantBits()
{
    return mValue2;
}

int PitchBend::getBendAmount()
{
    auto y = (mValue2 & 127) << 7;
    auto x = (mValue1);
    return y + x;
}

void PitchBend::setLeastSignificantBits(int p)
{
    mValue1 = p & 127;
}

void PitchBend::setMostSignificantBits(int p)
{
    mValue2 = p & 127;
}

void PitchBend::setBendAmount(int amount)
{
    amount = amount & 16383;
    mValue1 = (amount & 127);
    mValue2 = amount >> 7;
}
