#include "sequencer/ProgramChangeEvent.hpp"

using namespace mpc::sequencer;

ProgramChangeEvent::ProgramChangeEvent(
    const std::function<std::pair<EventIndex, EventState>()> &getSnapshot,
    const std::function<void(TrackEventMessage &&)> &dispatch)
    : Event(getSnapshot, dispatch)
{
}

void ProgramChangeEvent::setProgram(const int i) const
{
    auto e = getSnapshot();
    e.second.programChangeProgramIndex = ProgramIndex(i);
    dispatch(UpdateEvent{e});
}

int ProgramChangeEvent::getProgram() const
{
    return getSnapshot().second.programChangeProgramIndex;
}
