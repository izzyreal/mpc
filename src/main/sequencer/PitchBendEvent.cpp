#include "sequencer/PitchBendEvent.hpp"

using namespace mpc::sequencer;

PitchBendEvent::PitchBendEvent(
    EventData *const ptr, const EventData &snapshot,
    const std::function<void(TrackMessage &&)> &dispatch)
    : EventRef(ptr, snapshot, dispatch)
{
}

void PitchBendEvent::setAmount(const int i) const
{
    auto e = snapshot;
    e.amount = std::clamp(i, -8192, 8191);
    dispatch(UpdateEvent{handle, e});
}

int PitchBendEvent::getAmount() const
{
    return snapshot.amount;
}
