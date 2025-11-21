#include "sequencer/Event.hpp"

#include "IntTypes.hpp"

using namespace mpc::sequencer;

Event::Event(const std::function<EventState()> &getSnapshot,
            const std::function<void(TrackEventMessage &&)>
                &dispatch)
    : getSnapshot(getSnapshot), dispatch(dispatch)
{
}

Event::Event(const Event &event)
{
    setTick(event.getTick());
    Event::setTrack(event.getTrack());
    getSnapshot = event.getSnapshot;
    dispatch = event.dispatch;
}

void Event::setTick(const int tick) const
{
    dispatch(UpdateEventTick{getSnapshot(), tick});
}

int Event::getTick() const
{
    return getSnapshot().tick;
}

mpc::TrackIndex Event::getTrack() const
{
    return getSnapshot().trackIndex;
}

void Event::setTrack(const TrackIndex i)
{
    auto e = getSnapshot();
    e.trackIndex = i;
    dispatch(UpdateEvent{e});
}
