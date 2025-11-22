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
            const std::function<EventState()> &getSnapshot,
            const std::function<void(TrackEventMessage &&)> &dispatch);

        std::string getTypeName() const override
        {
            return "channel-pressure";
        }
    };
} // namespace mpc::sequencer
