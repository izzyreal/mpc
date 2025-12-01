#pragma once
#include <sequencer/EventRef.hpp>

namespace mpc::sequencer
{
    class PitchBendEvent final : public EventRef
    {
    public:
        void setAmount(int i) const;
        int getAmount() const;

        explicit PitchBendEvent(
            EventData *ptr, const EventData &snapshot,
            const std::function<void(TrackMessage &&)> &dispatch);

        std::string getTypeName() const override
        {
            return "pitch-bend";
        }
    };
} // namespace mpc::sequencer
