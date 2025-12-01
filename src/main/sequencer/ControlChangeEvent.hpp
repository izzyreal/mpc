#pragma once
#include <sequencer/EventRef.hpp>

namespace mpc::sequencer
{
    class ControlChangeEvent final : public EventRef
    {
    public:
        void setController(int i) const;
        int getController() const;
        void setAmount(int i) const;
        int getAmount() const;

        explicit ControlChangeEvent(
            EventData *ptr, const EventData &snapshot,
            const std::function<void(TrackMessage &&)> &dispatch);

        std::string getTypeName() const override
        {
            return "control-change";
        }
    };
} // namespace mpc::sequencer
