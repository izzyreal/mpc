#pragma once
#include <sequencer/Event.hpp>

namespace mpc::sequencer
{
    class ProgramChangeEvent final : public Event
    {
    public:
        void setProgram(int i) const;
        int getProgram() const;

        explicit ProgramChangeEvent(
            const std::function<EventState()>
                &getSnapshot,
            const std::function<void(TrackEventMessage &&)> &dispatch);

        std::string getTypeName() const override
        {
            return "program-change";
        }
    };
} // namespace mpc::sequencer
