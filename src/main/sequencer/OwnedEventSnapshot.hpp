#pragma once
#include "EventData.hpp"

namespace mpc::sequencer
{
    struct OwnedEventSnapshot : EventData
    {
        SysExPayloadOwner owner;
        OwnedEventSnapshot() = default;
        OwnedEventSnapshot(const EventData &data)
            : EventData(data), owner(data.sysExPayload)
        {
            sysExPayload = owner.get();
            sysExStored = false;
        }
        OwnedEventSnapshot(EventData data, SysExPayloadOwner payload)
            : EventData(data), owner(std::move(payload))
        {
            sysExPayload = owner.get();
            sysExStored = false;
        }
        OwnedEventSnapshot(const OwnedEventSnapshot &o)
            : EventData(o), owner(o.owner)
        {
            sysExPayload = owner.get();
            sysExStored = false;
        }
        OwnedEventSnapshot(OwnedEventSnapshot &&o) noexcept
            : EventData(o), owner(std::move(o.owner))
        {
            sysExPayload = owner.get();
            sysExStored = false;
            o.sysExPayload = nullptr;
        }
        OwnedEventSnapshot &operator=(OwnedEventSnapshot o) noexcept
        {
            static_cast<EventData &>(*this) = o;
            owner = std::move(o.owner);
            sysExPayload = owner.get();
            sysExStored = false;
            return *this;
        }
        bool setSysExBytes(SysExPool &pool, const uint8_t *bytes, size_t count)
        {
            type = EventType::SystemExclusive;
            owner = SysExPayloadOwner(pool.acquire(bytes, count));
            sysExPayload = owner.get();
            sysExStored = false;
            return sysExPayload != nullptr;
        }
        // SMF and legacy sequence bodies omit the F0 status byte.
        bool setMidiSysExBytes(SysExPool &pool, const uint8_t *bytes,
                               size_t count, bool appendTerminator = false)
        {
            const bool needsTerminator =
                appendTerminator && (count == 0 || bytes[count - 1] != 0xF7);
            if (count + 1 + needsTerminator > SysExPool::MaxBytes)
            {
                pool.reportFull();
                type = EventType::SystemExclusive;
                return false;
            }
            uint8_t message[SysExPool::MaxBytes] = {0xF0};
            std::memcpy(message + 1, bytes, count);
            if (needsTerminator)
            {
                message[count + 1] = 0xF7;
            }
            return setSysExBytes(pool, message, count + 1 + needsTerminator);
        }
        bool usable() const
        {
            return type != EventType::SystemExclusive || sysExPayload;
        }
    };
} // namespace mpc::sequencer
