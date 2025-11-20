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

        explicit ProgramChangeEvent(const std::function<performance::Event()> &getSnapshot);
        ProgramChangeEvent(const ProgramChangeEvent &event);

        std::string getTypeName() const override
        {
            return "program-change";
        }
    };
} // namespace mpc::sequencer
