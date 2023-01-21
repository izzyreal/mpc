#include <sequencer/Event.hpp>

using namespace mpc::sequencer;

void Event::setTick(int relativeTick)
{
    tick = relativeTick;
	
	notifyObservers(std::string("tick"));
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

void Event::CopyValuesTo(std::weak_ptr<Event> dest) {
	auto lDest = dest.lock();
	lDest->setTick(getTick());
	lDest->setTrack(getTrack());
}
