#include "sequencer/Event.hpp"

#include "IntTypes.hpp"

using namespace mpc::sequencer;

Event::Event(EventState *eventState,
             const std::function<void(SequenceMessage &&)> &dispatch)
    : eventState(eventState), dispatch(dispatch)
{
}

Event::Event(const Event &event)
{
    eventState = event.eventState;
    dispatch = event.dispatch;
}

void Event::setTick(const int tick) const
{
    dispatch(UpdateEventTick{eventState, tick});
}

int Event::getTick() const
{
    return eventState->tick;
}

mpc::TrackIndex Event::getTrack() const
{
    return eventState->trackIndex;
}

void Event::setTrack(const TrackIndex i) const
{
    auto e = *eventState;
    e.trackIndex = i;
    dispatch(UpdateEvent{eventState, e});
}
