#include <sequencer/PitchBendEvent.hpp>

using namespace mpc::sequencer;
using namespace std;

PitchBendEvent::PitchBendEvent()
{
}

void PitchBendEvent::setAmount(int i)
{
    if(i < -8192 || i > 8191) return;

    pitchBendAmount = i;
    setChanged();
    notifyObservers(string("step-editor"));
}

int PitchBendEvent::getAmount()
{
    return pitchBendAmount;
}

void PitchBendEvent::CopyValuesTo(std::weak_ptr<Event> dest) {
	Event::CopyValuesTo(dest);
	auto lDest = dynamic_pointer_cast<PitchBendEvent>(dest.lock());
	lDest->setAmount(getAmount());
}
