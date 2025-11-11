#include "sequencer/ProgramChangeEvent.hpp"

using namespace mpc::sequencer;

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

ProgramChangeEvent::ProgramChangeEvent(const ProgramChangeEvent &event)
    : Event(event)
{
    setProgram(event.getProgram());
}
