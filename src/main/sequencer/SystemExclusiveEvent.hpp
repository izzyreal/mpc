#pragma once
#include <sequencer/EventRef.hpp>

namespace mpc::sequencer
{
    class SystemExclusiveEvent final : public EventRef
    {
    public:
        void setByteA(int i) const;
        unsigned char getByteA() const;
        void setByteB(int i) const;
        unsigned char getByteB() const;
        void setBytes(const std::vector<unsigned char> &ba) const;
        size_t getByteCount() const
        {
            return snapshot.getSysExByteCount();
        }
        unsigned char getByte(size_t i) const
        {
            return snapshot.getSysExByte(i);
        }
        void setByte(size_t i, int value) const;

        explicit SystemExclusiveEvent(
            EventData *ptr, const EventData &snapshot,
            const std::function<void(TrackMessage &&)> &dispatch);
        std::string getTypeName() const override
        {
            return "system-exclusive";
        }
    };
} // namespace mpc::sequencer
