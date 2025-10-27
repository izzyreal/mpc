#pragma once
#include <sequencer/Event.hpp>

namespace mpc::sequencer
{
    class PitchBendEvent : public Event
    {

    private:
        int pitchBendAmount{0};

    public:
        void setAmount(int i);
        int getAmount() const;

        PitchBendEvent() = default;
        PitchBendEvent(const PitchBendEvent &);

        std::string getTypeName() override
        {
            return "pitch-bend";
        }
    };
} // namespace mpc::sequencer
