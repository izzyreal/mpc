#include "sequencer/ProgramChangeEvent.hpp"

using namespace mpc::sequencer;

ProgramChangeEvent::ProgramChangeEvent(
    EventState *eventState,
    const std::function<void(SequenceMessage &&)> &dispatch)
    : Event(eventState, dispatch)
{
}

void ProgramChangeEvent::setProgram(const int i) const
{
    auto e = *eventState;
    e.programChangeProgramIndex = ProgramIndex(i);
    dispatch(UpdateEvent{eventState, e});
}

int ProgramChangeEvent::getProgram() const
{
    return eventState->programChangeProgramIndex;
}
