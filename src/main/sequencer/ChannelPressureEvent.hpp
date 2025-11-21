#pragma once
#include <sequencer/Event.hpp>

namespace mpc::sequencer
{
    class ChannelPressureEvent final : public Event
    {
        int channelPressureValue{0};

    public:
        void setAmount(int i);
        int getAmount() const;

        explicit ChannelPressureEvent(
            const std::function<EventState()> &getSnapshot,
            const std::function<void(TrackEventMessage &&)> &dispatch);

        ChannelPressureEvent(const ChannelPressureEvent &event);

        std::string getTypeName() const override
        {
            return "channel-pressure";
        }
    };
} // namespace mpc::sequencer
