#pragma once
#include <sequencer/EventRef.hpp>

namespace mpc::sequencer
{
    class ChannelPressureEvent final : public EventRef
    {
    public:
        void setAmount(int i) const;
        int getAmount() const;

        explicit ChannelPressureEvent(
            EventData *ptr, const EventData &snapshot,
            const std::function<void(TrackMessage &&)> &dispatch);

        std::string getTypeName() const override
        {
            return "channel-pressure";
        }
    };
} // namespace mpc::sequencer
