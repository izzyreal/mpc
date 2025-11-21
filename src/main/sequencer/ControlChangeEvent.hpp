#pragma once
#include <sequencer/Event.hpp>

namespace mpc::sequencer
{
    class ControlChangeEvent final : public Event
    {
        int controllerNumber = 0;
        int controllerValue = 0;

    public:
        void setController(int i);
        int getController() const;
        void setAmount(int i);
        int getAmount() const;

        explicit ControlChangeEvent(
            const std::function<sequencer::EventState()> &getSnapshot);
        ControlChangeEvent(const ControlChangeEvent &event);

        std::string getTypeName() const override
        {
            return "control-change";
        }
    };
} // namespace mpc::sequencer
