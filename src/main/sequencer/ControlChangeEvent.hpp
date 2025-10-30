#pragma once
#include <sequencer/Event.hpp>

namespace mpc::sequencer
{
    class ControlChangeEvent : public Event
    {

    private:
        int controllerNumber = 0;
        int controllerValue = 0;

    public:
        void setController(int i);
        int getController() const;
        void setAmount(int i);
        int getAmount() const;

        ControlChangeEvent() = default;
        ControlChangeEvent(const ControlChangeEvent &event);

        std::string getTypeName() const override
        {
            return "control-change";
        }
    };
} // namespace mpc::sequencer
