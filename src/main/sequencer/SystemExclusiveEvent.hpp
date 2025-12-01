#pragma once
#include <sequencer/EventRef.hpp>

namespace mpc::sequencer
{
    class SystemExclusiveEvent final : public EventRef
    {
    public:
        void setByteA(unsigned char i);
        unsigned char getByteA() const;
        void setByteB(unsigned char i);
        unsigned char getByteB() const;
        void setBytes(const std::vector<unsigned char> &ba);
        const std::vector<unsigned char> &getBytes() const;

        explicit SystemExclusiveEvent(
            EventData *ptr, const EventData &snapshot,
            const std::function<void(TrackMessage &&)> &dispatch);
        std::string getTypeName() const override
        {
            return "system-exclusive";
        }
    };
} // namespace mpc::sequencer
