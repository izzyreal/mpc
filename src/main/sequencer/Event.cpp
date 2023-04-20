#include <sequencer/Event.hpp>

using namespace mpc::sequencer;

void Event::setTick(int relativeTick)
{
    tick = relativeTick;
	
	notifyObservers(std::string("tick"));
}

int Event::getTick() const
{
    return tick;
}

int Event::getTrack() const
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

Event::Event(const Event& event)
{
	setTick(event.getTick());
	setTrack(event.getTrack());
}
