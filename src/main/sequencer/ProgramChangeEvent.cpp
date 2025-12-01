#include "sequencer/ProgramChangeEvent.hpp"

using namespace mpc::sequencer;

ProgramChangeEvent::ProgramChangeEvent(
    EventData *const ptr, const EventData &snapshot,
    const std::function<void(TrackMessage &&)> &dispatch)
    : EventRef(ptr, snapshot, dispatch)
{
}

void ProgramChangeEvent::setProgram(const int i) const
{
    auto e = snapshot;
    e.programChangeProgramIndex = ProgramIndex(i);
    dispatch(UpdateEvent{handle, e});
}

int ProgramChangeEvent::getProgram() const
{
    return snapshot.programChangeProgramIndex;
}
