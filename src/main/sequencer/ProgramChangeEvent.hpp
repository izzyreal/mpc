#pragma once
#include <sequencer/Event.hpp>

namespace mpc::sequencer
{
    class ProgramChangeEvent : public Event
    {

    private:
        int programChangeValue = 0;

    public:
        void setProgram(int i);
        int getProgram() const;

        ProgramChangeEvent() = default;
        ProgramChangeEvent(const ProgramChangeEvent &event);

        std::string getTypeName() const override
        {
            return "program-change";
        }
    };
} // namespace mpc::sequencer
