#pragma once
#include <sequencer/Event.hpp>

namespace mpc::sequencer
{
    class ChannelPressureEvent final : public Event
    {
    public:
        void setAmount(int i) const;
        int getAmount() const;

        explicit ChannelPressureEvent(
            EventState *eventState,
            const std::function<void(SequenceMessage &&)> &dispatch);

        std::string getTypeName() const override
        {
            return "channel-pressure";
        }
    };
} // namespace mpc::sequencer
