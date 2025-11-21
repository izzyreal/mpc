#include "sequencer/SystemExclusiveEvent.hpp"

using namespace mpc::sequencer;

SystemExclusiveEvent::SystemExclusiveEvent(
    const std::function<EventState()> &getSnapshot, const std::function<void(TrackEventMessage &&)> &dispatch)
    : Event(getSnapshot, dispatch)
{
}

SystemExclusiveEvent::SystemExclusiveEvent(const SystemExclusiveEvent &event)
    : Event(event)
{
    setBytes(event.getBytes());
}

void SystemExclusiveEvent::setByteA(const unsigned char i)
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

void SystemExclusiveEvent::setByteB(const unsigned char i)
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
