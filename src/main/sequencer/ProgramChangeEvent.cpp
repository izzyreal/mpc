#include "sequencer/ProgramChangeEvent.hpp"

using namespace mpc::sequencer;

ProgramChangeEvent::ProgramChangeEvent(
    const std::function<EventState()> &getSnapshot,
    const std::function<void(SequencerMessage &&)> &dispatch)
    : Event(getSnapshot, dispatch)
{
}

void ProgramChangeEvent::setProgram(const int i) const
{
    auto e = getSnapshot();
    e.programChangeProgramIndex = ProgramIndex(i);
    dispatch(UpdateEvent{e});
}

int ProgramChangeEvent::getProgram() const
{
    return getSnapshot().programChangeProgramIndex;
}
