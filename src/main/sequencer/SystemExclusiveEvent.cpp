#include "sequencer/SystemExclusiveEvent.hpp"

#include <algorithm>

using namespace mpc::sequencer;

SystemExclusiveEvent::SystemExclusiveEvent(
    EventData *const ptr, const EventData &snapshot,
    const std::function<void(TrackMessage &&)> &dispatch)
    : EventRef(ptr, snapshot, dispatch)
{
}

void SystemExclusiveEvent::setByteA(const int i) const
{
    auto e = snapshot;
    e.sysExByteA = static_cast<uint8_t>(std::clamp(i, 0, 255));
    dispatch(UpdateEvent{handle, e});
}

unsigned char SystemExclusiveEvent::getByteA() const
{
    return snapshot.sysExByteA;
}

void SystemExclusiveEvent::setByteB(const int i) const
{
    auto e = snapshot;
    e.sysExByteB = static_cast<uint8_t>(std::clamp(i, 0, 255));
    dispatch(UpdateEvent{handle, e});
}

unsigned char SystemExclusiveEvent::getByteB() const
{
    return snapshot.sysExByteB;
}

void SystemExclusiveEvent::setBytes(const std::vector<unsigned char> &ba) const
{
    auto e = snapshot;
    e.sysExByteA = ba.size() > 0 ? ba[0] : 0;
    e.sysExByteB = ba.size() > 1 ? ba[1] : 0;
    dispatch(UpdateEvent{handle, e});
}
