#include "sequencer/PitchBendEvent.hpp"

using namespace mpc::sequencer;

void PitchBendEvent::setAmount(int i)
{
    if(i < -8192 || i > 8191) return;

    pitchBendAmount = i;
    
    notifyObservers(std::string("step-editor"));
}

int PitchBendEvent::getAmount() const
{
    return pitchBendAmount;
}

mpc::sequencer::PitchBendEvent::PitchBendEvent(const PitchBendEvent& event) : Event(event)
{
    setAmount(event.getAmount());
}
