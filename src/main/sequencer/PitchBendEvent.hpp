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

        explicit PitchBendEvent(
            const std::function<EventState()> &getSnapshot, const std::function<void(TrackEventMessage &&)> &dispatch);
        PitchBendEvent(const PitchBendEvent &);

        std::string getTypeName() const override
        {
            return "pitch-bend";
        }
    };
} // namespace mpc::sequencer
