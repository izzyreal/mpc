#include "sequencer/MixerEvent.hpp"

using namespace mpc::sequencer;

MixerEvent::MixerEvent(EventData *const ptr, const EventData &snapshot,
                       const std::function<void(TrackMessage &&)> &dispatch)
    : EventRef(ptr, snapshot, dispatch)
{
}

void MixerEvent::setParameter(const int i) const
{
    auto e = snapshot;
    e.mixerParameter = i;
    dispatch(UpdateEvent{handle, e});
}

int MixerEvent::getParameter() const
{
    return snapshot.mixerParameter;
}

void MixerEvent::setPadNumber(const int i) const
{
    auto e = snapshot;
    e.mixerPad = i;
    dispatch(UpdateEvent{handle, e});
}

int MixerEvent::getPad() const
{
    return snapshot.mixerPad;
}

void MixerEvent::setValue(const int i) const
{
    auto e = snapshot;
    e.mixerValue = i;
    dispatch(UpdateEvent{handle, e});
}

int MixerEvent::getValue() const
{
    return snapshot.mixerValue;
}
