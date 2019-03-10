#include <sequencer/MidiClockEvent.hpp>

using namespace mpc::sequencer;
using namespace std;

MidiClockEvent::MidiClockEvent(int status) 
{
	this->status = status;
}

int MidiClockEvent::getStatus()
{
    return status;
}

void MidiClockEvent::setStatus(int i)
{
    status = i;
}

void MidiClockEvent::CopyValuesTo(std::weak_ptr<Event> dest) {
	auto lDest = dynamic_pointer_cast<MidiClockEvent>(dest.lock());
	Event::CopyValuesTo(dest);
	lDest->setStatus(getStatus());
}
