#include "sequencer/SystemExclusiveEvent.hpp"

using namespace mpc::sequencer;

SystemExclusiveEvent::SystemExclusiveEvent(
    EventData *const ptr, const EventData &snapshot,
    const std::function<void(TrackMessage &&)> &dispatch)
    : EventRef(ptr, snapshot, dispatch)
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
