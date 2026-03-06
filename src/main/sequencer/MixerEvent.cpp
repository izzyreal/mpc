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
    e.mixerParameter = static_cast<int8_t>(std::clamp(i, 0, 3));
    dispatch(UpdateEvent{handle, e});
}

int MixerEvent::getParameter() const
{
    return snapshot.mixerParameter;
}

void MixerEvent::setPadNumber(const int i) const
{
    auto e = snapshot;
    e.mixerPad =
        static_cast<int8_t>(std::clamp(i, static_cast<int>(MinProgramPadIndex),
                                       static_cast<int>(MaxProgramPadIndex)));
    dispatch(UpdateEvent{handle, e});
}

int MixerEvent::getPad() const
{
    return snapshot.mixerPad;
}

void MixerEvent::setValue(const int i) const
{
    auto e = snapshot;
    e.mixerValue = static_cast<int8_t>(std::clamp(i, 0, 100));
    dispatch(UpdateEvent{handle, e});
}

int MixerEvent::getValue() const
{
    return snapshot.mixerValue;
}
