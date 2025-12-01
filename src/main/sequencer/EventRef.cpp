#include "sequencer/EventRef.hpp"

#include "IntTypes.hpp"

using namespace mpc::sequencer;

EventRef::EventRef(EventData *const ptr, const EventData &snapshot,
                   const std::function<void(TrackMessage &&)> &dispatch)
    : handle(ptr), snapshot(snapshot), dispatch(dispatch)
{
}

EventRef::EventRef(const EventRef &event)
    : handle(event.handle), snapshot(event.snapshot), dispatch(event.dispatch)
{
}

void EventRef::setTick(const int tick) const
{
    dispatch(UpdateEventTick{handle, tick});
}

int EventRef::getTick() const
{
    return snapshot.tick;
}

void EventRef::setTrack(const TrackIndex i) const
{
    auto e = snapshot;
    e.trackIndex = i;
    dispatch(UpdateEvent{handle, e});
}

mpc::TrackIndex EventRef::getTrack() const
{
    return snapshot.trackIndex;
}
