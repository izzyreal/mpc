#include "sequencer/MixerEvent.hpp"

using namespace mpc::sequencer;

MixerEvent::MixerEvent(EventState *eventState,
                       const std::function<void(TrackMessage &&)> &dispatch)
    : Event(eventState, dispatch)
{
}

void MixerEvent::setParameter(const int i) const
{
    auto e = *eventState;
    e.mixerParameter = i;
    dispatch(UpdateEvent{eventState, e});
}

int MixerEvent::getParameter() const
{
    return eventState->mixerParameter;
}

void MixerEvent::setPadNumber(const int i) const
{
    auto e = *eventState;
    e.mixerPad = i;
    dispatch(UpdateEvent{eventState, e});
}

int MixerEvent::getPad() const
{
    return eventState->mixerPad;
}

void MixerEvent::setValue(const int i) const
{
    auto e = *eventState;
    e.mixerValue = i;
    dispatch(UpdateEvent{eventState, e});
}

int MixerEvent::getValue() const
{
    return eventState->mixerValue;
}
