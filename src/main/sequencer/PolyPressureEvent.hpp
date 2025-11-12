#pragma once
#include <sequencer/Event.hpp>

namespace mpc::sequencer
{
    class PolyPressureEvent : public Event
    {

        int note{0};
        int polyPressureValue{0};

    public:
        void setNote(int i);
        int getNote() const;
        void setAmount(int i);
        int getAmount() const;

        PolyPressureEvent() = default;
        PolyPressureEvent(const PolyPressureEvent &event);

        std::string getTypeName() const override
        {
            return "poly-pressure";
        }
    };
} // namespace mpc::sequencer
