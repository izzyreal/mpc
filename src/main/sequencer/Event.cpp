#include "sequencer/Event.hpp"

#include "IntTypes.hpp"

using namespace mpc::sequencer;

Event::Event(const std::function<std::pair<EventIndex, EventState>()> &getSnapshot,
             const std::function<void(TrackEventMessage &&)> &dispatch)
    : getSnapshot(getSnapshot), dispatch(dispatch)
{
}

Event::Event(const Event &event)
{
    getSnapshot = event.getSnapshot;
    dispatch = event.dispatch;
}

void Event::setTick(const int tick) const
{
    dispatch(UpdateEventTick{getSnapshot().first, tick});
}

int Event::getTick() const
{
    return getSnapshot().second.tick;
}

mpc::TrackIndex Event::getTrack() const
{
    return getSnapshot().second.trackIndex;
}

void Event::setTrack(const TrackIndex i) const
{
    auto e = getSnapshot();
    e.second.trackIndex = i;
    dispatch(UpdateEvent{e});
}
