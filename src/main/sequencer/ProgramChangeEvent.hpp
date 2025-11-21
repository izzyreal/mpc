#pragma once
#include <sequencer/Event.hpp>

namespace mpc::sequencer
{
    class ProgramChangeEvent final : public Event
    {
        int programChangeValue = 0;

    public:
        void setProgram(int i);
        int getProgram() const;

        explicit ProgramChangeEvent(
            const std::function<EventState()> &getSnapshot, const std::function<void(TrackEventMessage &&)> &dispatch);
        ProgramChangeEvent(const ProgramChangeEvent &event);

        std::string getTypeName() const override
        {
            return "program-change";
        }
    };
} // namespace mpc::sequencer
