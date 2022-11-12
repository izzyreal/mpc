#include <sequencer/MidiClockEvent.hpp>

using namespace mpc::sequencer;

MidiClockEvent::MidiClockEvent(int statusToUse) : status(statusToUse)
{
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
	auto lDest = std::dynamic_pointer_cast<MidiClockEvent>(dest.lock());
	Event::CopyValuesTo(dest);
	lDest->setStatus(getStatus());
}
