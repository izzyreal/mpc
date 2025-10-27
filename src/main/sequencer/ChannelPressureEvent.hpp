#pragma once
#include <sequencer/Event.hpp>

#include <memory>

namespace mpc::sequencer
{
    class ChannelPressureEvent : public Event
    {

    private:
        int channelPressureValue{0};

    public:
        void setAmount(int i);
        int getAmount() const;

        ChannelPressureEvent() = default;
        ChannelPressureEvent(const ChannelPressureEvent &event);

        std::string getTypeName() override
        {
            return "channel-pressure";
        }
    };
} // namespace mpc::sequencer
