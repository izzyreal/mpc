#include "sequencer/ProgramChangeEvent.hpp"

using namespace mpc::sequencer;

void ProgramChangeEvent::setProgram(int i)
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

mpc::sequencer::ProgramChangeEvent::ProgramChangeEvent(
    const ProgramChangeEvent &event)
    : Event(event)
{
    setProgram(event.getProgram());
}
