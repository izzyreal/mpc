#include <sequencer/Event.hpp>
#include <midi/core/MidiMessage.hpp>
#include <midi/core/ShortMessage.hpp>

using namespace mpc::sequencer;
using namespace std;

Event::Event()
{
	shortMessage = new ctoot::midi::core::ShortMessage();
}

void Event::setTick(int relativeTick)
{
    tick = relativeTick;
	setChanged();
	notifyObservers(string("tick"));
}

int Event::getTick()
{
    return tick;
}

int Event::getTrack()
{
    return track;
}

void Event::setTrack(int i)
{
    track = i;
}

void Event::CopyValuesTo(weak_ptr<Event> dest) {
	auto lDest = dest.lock();
	lDest->setTick(getTick());
	lDest->setTrack(getTrack());
}

ctoot::midi::core::ShortMessage* Event::getShortMessage()
{
	return shortMessage;
}


Event::~Event() {
	delete shortMessage;
}
