#pragma once
#include <sequencer/Event.hpp>

namespace mpc::sequencer
{
    class PolyPressureEvent final : public Event
    {
        int note{0};
        int polyPressureValue{0};

    public:
        void setNote(int i);
        int getNote() const;
        void setAmount(int i);
        int getAmount() const;

        explicit PolyPressureEvent(const std::function<sequencer::EventState()> &getSnapshot);
        PolyPressureEvent(const PolyPressureEvent &event);

        std::string getTypeName() const override
        {
            return "poly-pressure";
        }
    };
} // namespace mpc::sequencer
