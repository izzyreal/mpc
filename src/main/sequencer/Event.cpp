#include "sequencer/Event.hpp"

using namespace mpc::sequencer;

void Event::setTick(const int relativeTick)
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

void Event::setTrack(const int i)
{
    track = i;
}

Event::Event(const Event &event)
{
    setTick(event.getTick());
    setTrack(event.getTrack());
}
