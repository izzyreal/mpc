#include "sequencer/MixerEvent.hpp"

using namespace mpc::sequencer;

MixerEvent::MixerEvent(
    const std::function<EventState()> &getSnapshot,
    const std::function<void(SequencerMessage &&)> &dispatch)
    : Event(getSnapshot, dispatch)
{
}

void MixerEvent::setParameter(const int i) const
{
    auto e = getSnapshot();
    e.mixerParameter = i;
    dispatch(UpdateEvent{e});
}

int MixerEvent::getParameter() const
{
    return getSnapshot().mixerParameter;
}

void MixerEvent::setPadNumber(const int i) const
{
    auto e = getSnapshot();
    e.mixerPad = i;
    dispatch(UpdateEvent{e});
}

int MixerEvent::getPad() const
{
    return getSnapshot().mixerPad;
}

void MixerEvent::setValue(const int i) const
{
    auto e = getSnapshot();
    e.mixerValue = i;
    dispatch(UpdateEvent{e});
}

int MixerEvent::getValue() const
{
    return getSnapshot().mixerValue;
}
