#include "sequencer/SystemExclusiveEvent.hpp"

using namespace mpc::sequencer;

void SystemExclusiveEvent::setByteA(unsigned char i)
{
    if (i < 0 || i > 255)
    {
        return;
    }

    bytes[0] = i;
}

unsigned char SystemExclusiveEvent::getByteA() const
{
    return bytes[0];
}

void SystemExclusiveEvent::setByteB(unsigned char i)
{
    if (i < 0 || i > 255)
    {
        return;
    }

    bytes[1] = i;
}

unsigned char SystemExclusiveEvent::getByteB() const
{
    return bytes[1];
}

void SystemExclusiveEvent::setBytes(const std::vector<unsigned char> &ba)
{
    bytes = ba;
}

const std::vector<unsigned char> &SystemExclusiveEvent::getBytes() const
{
    return bytes;
}

mpc::sequencer::SystemExclusiveEvent::SystemExclusiveEvent(
    const SystemExclusiveEvent &event)
    : Event(event)
{
    setBytes(event.getBytes());
}
