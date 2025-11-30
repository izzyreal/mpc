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
            EventState *eventState,
            const std::function<void(SequenceMessage &&)> &dispatch);

        std::string getTypeName() const override
        {
            return "program-change";
        }
    };
} // namespace mpc::sequencer
