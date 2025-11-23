#include "sequencer/Event.hpp"

#include "IntTypes.hpp"

using namespace mpc::sequencer;

Event::Event(const std::function<EventState()> &getSnapshot,
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
    dispatch(UpdateEventTick{getSnapshot().eventId, tick});
}

int Event::getTick() const
{
    return getSnapshot().tick;
}

mpc::TrackIndex Event::getTrack() const
{
    return getSnapshot().trackIndex;
}

void Event::setTrack(const TrackIndex i) const
{
    auto e = getSnapshot();
    e.trackIndex = i;
    dispatch(UpdateEvent{e});
}
