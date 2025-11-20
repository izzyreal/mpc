#include "sequencer/Event.hpp"

#include "IntTypes.hpp"

using namespace mpc::sequencer;

void Event::setTick(const int relativeTick)
{
    tick = relativeTick;

    // notifyObservers(std::string("tick"));
}

int Event::getTick() const
{
    return tick;
}

mpc::TrackIndex Event::getTrack() const
{
    return track;
}

void Event::setTrack(const TrackIndex i)
{
    track = i;
}

Event::Event(const Event &event)
{
    setTick(event.getTick());
    Event::setTrack(event.getTrack());
}
