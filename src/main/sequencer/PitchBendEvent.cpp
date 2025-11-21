#include "sequencer/PitchBendEvent.hpp"

using namespace mpc::sequencer;

PitchBendEvent::PitchBendEvent(
    const std::function<sequencer::EventState()> &getSnapshot)
    : Event(getSnapshot)
{
}

PitchBendEvent::PitchBendEvent(const PitchBendEvent &event) : Event(event)
{
    setAmount(event.getAmount());
}

void PitchBendEvent::setAmount(const int i)
{
    if (i < -8192 || i > 8191)
    {
        return;
    }

    pitchBendAmount = i;
}

int PitchBendEvent::getAmount() const
{
    return pitchBendAmount;
}
