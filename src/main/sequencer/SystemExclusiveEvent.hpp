#pragma once
#include <sequencer/Event.hpp>

namespace mpc::sequencer
{
    class SystemExclusiveEvent final : public Event
    {
    public:
        void setByteA(unsigned char i);
        unsigned char getByteA() const;
        void setByteB(unsigned char i);
        unsigned char getByteB() const;
        void setBytes(const std::vector<unsigned char> &ba);
        const std::vector<unsigned char> &getBytes() const;

        explicit SystemExclusiveEvent(
            const std::function<std::pair<EventId, EventState>()>
                &getSnapshot,
            const std::function<void(TrackEventMessage &&)> &dispatch);
        std::string getTypeName() const override
        {
            return "system-exclusive";
        }
    };
} // namespace mpc::sequencer
