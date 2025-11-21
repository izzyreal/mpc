#pragma once
#include <sequencer/Event.hpp>

namespace mpc::sequencer
{
    class PitchBendEvent final : public Event
    {

        int pitchBendAmount{0};

    public:
        void setAmount(int i);
        int getAmount() const;

        PitchBendEvent(
            const std::function<sequencer::EventState()> &getSnapshot);
        PitchBendEvent(const PitchBendEvent &);

        std::string getTypeName() const override
        {
            return "pitch-bend";
        }
    };
} // namespace mpc::sequencer
