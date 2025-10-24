#pragma once
#include <sequencer/Event.hpp>

namespace mpc::sequencer
{
    class SystemExclusiveEvent
        : public Event
    {

    private:
        std::vector<unsigned char> bytes = std::vector<unsigned char>(2);

    public:
        void setByteA(unsigned char i);
        unsigned char getByteA();
        void setByteB(unsigned char i);
        unsigned char getByteB();
        void setBytes(const std::vector<unsigned char> &ba);
        const std::vector<unsigned char> &getBytes() const;

        SystemExclusiveEvent() = default;
        SystemExclusiveEvent(const SystemExclusiveEvent &event);
        std::string getTypeName() override
        {
            return "system-exclusive";
        }
    };
} // namespace mpc::sequencer
