#pragma once
#include <sequencer/Event.hpp>

namespace mpc::sequencer
{
    class PolyPressureEvent final : public Event
    {
    public:
        void setNote(int i) const;
        int getNote() const;
        void setAmount(int i) const;
        int getAmount() const;

        explicit PolyPressureEvent(
            const std::function<EventState()> &getSnapshot,
            const std::function<void(NonRtSequencerMessage &&)> &dispatch);

        std::string getTypeName() const override
        {
            return "poly-pressure";
        }
    };
} // namespace mpc::sequencer
