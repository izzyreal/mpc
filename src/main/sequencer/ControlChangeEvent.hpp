#pragma once
#include <sequencer/Event.hpp>

namespace mpc::sequencer
{
    class ControlChangeEvent final : public Event
    {
    public:
        void setController(int i) const;
        int getController() const;
        void setAmount(int i) const;
        int getAmount() const;

        explicit ControlChangeEvent(
            const std::function<std::pair<EventIndex, EventState>()>
                &getSnapshot,
            const std::function<void(TrackEventMessage &&)> &dispatch);

        std::string getTypeName() const override
        {
            return "control-change";
        }
    };
} // namespace mpc::sequencer
