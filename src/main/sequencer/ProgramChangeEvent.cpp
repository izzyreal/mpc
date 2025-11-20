#include "sequencer/ProgramChangeEvent.hpp"

using namespace mpc::sequencer;

ProgramChangeEvent::ProgramChangeEvent(
    const std::function<performance::Event()> &getSnapshot)
        : Event(getSnapshot)
{
}

ProgramChangeEvent::ProgramChangeEvent(const ProgramChangeEvent &event)
    : Event(event)
{
    setProgram(event.getProgram());
}

void ProgramChangeEvent::setProgram(const int i)
{
    if (i < 1 || i > 128)
    {
        return;
    }
    programChangeValue = i;
}

int ProgramChangeEvent::getProgram() const
{
    return programChangeValue;
}
