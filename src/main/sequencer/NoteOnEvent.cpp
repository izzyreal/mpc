#include "sequencer/NoteOnEvent.hpp"

#include <algorithm>

using namespace mpc::sequencer;

NoteOnEvent::NoteOnEvent(EventData *const ptr, const EventData &snapshot,
                         const std::function<void(TrackMessage &&)> &dispatch,
                         const NoteNumber noteNumber, const Velocity vel)
    : EventRef(ptr, snapshot, dispatch)
{
    setNote(noteNumber);
    setVelocity(vel);
}

NoteOnEvent::NoteOnEvent(EventData *const ptr, const EventData &snapshot,
                         const std::function<void(TrackMessage &&)> &dispatch)
    : EventRef(ptr, snapshot, dispatch)
{
}

NoteOnEvent::NoteOnEvent(EventData *const ptr, const EventData &snapshot,
                         const std::function<void(TrackMessage &&)> &dispatch,
                         const DrumNoteNumber drumNoteNumber)
    : EventRef(ptr, snapshot, dispatch)
{
    setNote(drumNoteNumber);
}

void NoteOnEvent::setNote(const NoteNumber n) const
{
    auto e = snapshot;
    e.noteNumber = n;
    dispatch(UpdateEvent{handle, e});
}

mpc::NoteNumber NoteOnEvent::getNote() const
{
    return snapshot.noteNumber;
}

void NoteOnEvent::setDuration(const Duration duration) const
{
    auto e = snapshot;
    e.duration = std::clamp(duration, MinDuration, MaxDuration);
    dispatch(UpdateEvent{handle, e});
}

mpc::Duration NoteOnEvent::getDuration() const
{
    return snapshot.duration;
}

mpc::NoteVariationType NoteOnEvent::getVariationType() const
{
    return snapshot.noteVariationType;
}

void NoteOnEvent::setVariationType(const NoteVariationType type) const
{
    auto e = snapshot;
    e.noteVariationType = type;

    if (e.noteVariationType == NoteVariationTypeTune)
    {
        e.noteVariationValue =
            std::clamp(e.noteVariationValue, static_cast<NoteVariationValue>(4),
                       static_cast<NoteVariationValue>(124));
    }
    else
    {
        e.noteVariationValue =
            std::clamp(e.noteVariationValue, static_cast<NoteVariationValue>(0),
                       static_cast<NoteVariationValue>(100));
    }

    dispatch(UpdateEvent{handle, e});
}

void NoteOnEvent::setVariationValue(const int i) const
{
    auto e = snapshot;
    int8_t newValue;

    if (e.noteVariationType == NoteVariationTypeTune)
    {
        newValue = static_cast<int8_t>(std::clamp(i, 4, 124));
    }
    else
    {
        newValue = static_cast<int8_t>(std::clamp(i, 0, 100));
    }

    e.noteVariationValue = NoteVariationValue(newValue);
    dispatch(UpdateEvent{handle, e});
}

int NoteOnEvent::getVariationValue() const
{
    return snapshot.noteVariationValue;
}

void NoteOnEvent::setVelocity(const Velocity velocity) const
{
    auto e = snapshot;
    e.velocity = std::clamp(velocity, MinVelocity, MaxVelocity);
    dispatch(UpdateEvent{handle, e});
}

mpc::Velocity NoteOnEvent::getVelocity() const
{
    return snapshot.velocity;
}
