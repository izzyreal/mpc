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

        explicit PolyPressureEvent(
            const std::function<EventState()> &getSnapshot, const std::function<void(TrackEventMessage &&)> &dispatch);
        PolyPressureEvent(const PolyPressureEvent &event);

        std::string getTypeName() const override
        {
            return "poly-pressure";
        }
    };
} // namespace mpc::sequencer
