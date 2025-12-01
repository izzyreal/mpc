#pragma once
#include <sequencer/EventRef.hpp>

namespace mpc::sequencer
{
    class PolyPressureEvent final : public EventRef
    {
    public:
        void setNote(int i) const;
        int getNote() const;
        void setAmount(int i) const;
        int getAmount() const;

        explicit PolyPressureEvent(
            EventData *ptr, const EventData &snapshot,
            const std::function<void(TrackMessage &&)> &dispatch);

        std::string getTypeName() const override
        {
            return "poly-pressure";
        }
    };
} // namespace mpc::sequencer
