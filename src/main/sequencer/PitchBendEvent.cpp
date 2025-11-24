#include "sequencer/PitchBendEvent.hpp"

using namespace mpc::sequencer;

PitchBendEvent::PitchBendEvent(
    const std::function<EventState()> &getSnapshot,
    const std::function<void(NonRtSequencerMessage &&)> &dispatch)
    : Event(getSnapshot, dispatch)
{
}

void PitchBendEvent::setAmount(const int i) const
{
    auto e = getSnapshot();
    e.amount = std::clamp(i, -8192, 8191);
    dispatch(UpdateEvent{e});
}

int PitchBendEvent::getAmount() const
{
    return getSnapshot().amount;
}
