#include "sequencer/SystemExclusiveEvent.hpp"

#include <algorithm>

using namespace mpc::sequencer;

SystemExclusiveEvent::SystemExclusiveEvent(
    EventData *const ptr, const EventData &snapshot,
    const std::function<void(TrackMessage &&)> &dispatch)
    : EventRef(ptr, snapshot, dispatch)
{
}

void SystemExclusiveEvent::setByte(size_t i, int value) const
{
    if (!snapshot.sysExPayload || i >= getByteCount())
    {
        return;
    }
    if ((i == 0 && getByte(i) == 0xF0) ||
        (i == getByteCount() - 1 && getByte(i) == 0xF7))
    {
        return;
    }
    SysExPayloadOwner edited(snapshot.sysExPayload->pool->acquireEdited(
        snapshot.sysExPayload, i,
        static_cast<uint8_t>(std::clamp(value, 0, 0x7F))));
    if (!edited.get())
    {
        return;
    }
    OwnedEventSnapshot replacement(snapshot, std::move(edited));
    dispatch(UpdateEvent{handle, std::move(replacement)});
}
void SystemExclusiveEvent::setByteA(int value) const
{
    setByte(0, value);
}
void SystemExclusiveEvent::setByteB(int value) const
{
    setByte(1, value);
}
unsigned char SystemExclusiveEvent::getByteA() const
{
    return getByte(0);
}
unsigned char SystemExclusiveEvent::getByteB() const
{
    return getByte(1);
}
void SystemExclusiveEvent::setBytes(
    const std::vector<unsigned char> &bytes) const
{
    if (!snapshot.sysExPayload)
    {
        return;
    }
    SysExPayloadOwner payload(
        snapshot.sysExPayload->pool->acquire(bytes.data(), bytes.size()));
    if (!payload.get())
    {
        return;
    }
    dispatch(
        UpdateEvent{handle, OwnedEventSnapshot(snapshot, std::move(payload))});
}
