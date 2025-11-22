#include "sequencer/SystemExclusiveEvent.hpp"

using namespace mpc::sequencer;

SystemExclusiveEvent::SystemExclusiveEvent(
    const std::function<EventState()> &getSnapshot,
    const std::function<void(TrackEventMessage &&)> &dispatch)
    : Event(getSnapshot, dispatch)
{
}

void SystemExclusiveEvent::setByteA(const unsigned char i) {}

unsigned char SystemExclusiveEvent::getByteA() const
{
    return 0;
}

void SystemExclusiveEvent::setByteB(const unsigned char i) {}

unsigned char SystemExclusiveEvent::getByteB() const
{
    return 0;
}

void SystemExclusiveEvent::setBytes(const std::vector<unsigned char> &ba) {}

const std::vector<unsigned char> &SystemExclusiveEvent::getBytes() const
{
    static std::vector<unsigned char> empty;
    return empty;
}
