#pragma once
#include <sequencer/Event.hpp>

namespace mpc::sequencer
{
    class ChannelPressureEvent : public Event
    {
        int channelPressureValue{0};

    public:
        void setAmount(int i);
        int getAmount() const;

        ChannelPressureEvent() = default;
        ChannelPressureEvent(const ChannelPressureEvent &event);

        std::string getTypeName() const override
        {
            return "channel-pressure";
        }
    };
} // namespace mpc::sequencer
