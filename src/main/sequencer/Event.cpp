#include "sequencer/Event.hpp"

#include "IntTypes.hpp"

using namespace mpc::sequencer;

Event::Event(const std::function<sequencer::EventState()> &getSnapshot)
    : getSnapshot(getSnapshot)
{

}

Event::Event(const Event &event)
{
    setTick(event.getTick());
    Event::setTrack(event.getTrack());
    getSnapshot = event.getSnapshot;
}

void Event::setTick(const int relativeTick)
{
    // tick = relativeTick;
}

int Event::getTick() const
{
    return getSnapshot().tick;
}

mpc::TrackIndex Event::getTrack() const
{
    return track;
}

void Event::setTrack(const TrackIndex i)
{
    track = i;
}
