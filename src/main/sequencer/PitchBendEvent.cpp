#include "sequencer/PitchBendEvent.hpp"

using namespace mpc::sequencer;

PitchBendEvent::PitchBendEvent(
    EventState *eventState,
    const std::function<void(TrackMessage &&)> &dispatch)
    : Event(eventState, dispatch)
{
}

void PitchBendEvent::setAmount(const int i) const
{
    auto e = *eventState;
    e.amount = std::clamp(i, -8192, 8191);
    dispatch(UpdateEvent{eventState, e});
}

int PitchBendEvent::getAmount() const
{
    return eventState->amount;
}
