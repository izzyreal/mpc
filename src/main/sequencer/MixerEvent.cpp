#include "sequencer/MixerEvent.hpp"

using namespace mpc::sequencer;

MixerEvent::MixerEvent(
    const std::function<std::pair<EventId, EventState>()> &getSnapshot,
    const std::function<void(TrackEventMessage &&)> &dispatch)
    : Event(getSnapshot, dispatch)
{
}

void MixerEvent::setParameter(const int i) const
{
    auto e = getSnapshot();
    e.second.mixerParameter = i;
    dispatch(UpdateEvent{e});
}

int MixerEvent::getParameter() const
{
    return getSnapshot().second.mixerParameter;
}

void MixerEvent::setPadNumber(const int i) const
{
    auto e = getSnapshot();
    e.second.mixerPad = i;
    dispatch(UpdateEvent{e});
}

int MixerEvent::getPad() const
{
    return getSnapshot().second.mixerPad;
}

void MixerEvent::setValue(const int i) const
{
    auto e = getSnapshot();
    e.second.mixerValue = i;
    dispatch(UpdateEvent{e});
}

int MixerEvent::getValue() const
{
    return getSnapshot().second.mixerValue;
}
