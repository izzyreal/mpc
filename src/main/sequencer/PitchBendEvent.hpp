#pragma once
#include <sequencer/Event.hpp>

namespace mpc::sequencer
{
    class PitchBendEvent final : public Event
    {
    public:
        void setAmount(int i) const;
        int getAmount() const;

        explicit PitchBendEvent(
            EventState *eventState,
            const std::function<void(TrackMessage &&)> &dispatch);

        std::string getTypeName() const override
        {
            return "pitch-bend";
        }
    };
} // namespace mpc::sequencer
