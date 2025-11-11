#include "sequencer/NoteEvent.hpp"

#include <algorithm>
#include <cassert>

using namespace mpc::sequencer;

bool mpc::sequencer::isDrumNote(const int number)
{
    return number >= 35 && number <= 98;
}

void NoteOffEvent::setNote(const int i)
{
    number = i;
}

int NoteOffEvent::getNote() const
{
    return number;
}

NoteOnEvent::NoteOnEvent(const int i, const int vel)
{
    noteOff = std::shared_ptr<NoteOffEvent>(new NoteOffEvent());
    setNote(i);
    setVelocity(vel);
}

NoteOnEvent::NoteOnEvent(const NoteOnEvent &event) : Event(event)
{
    noteOff = std::shared_ptr<NoteOffEvent>(new NoteOffEvent());
    setNote(event.number);
    setVelocity(event.velocity);
    setDuration(event.duration);
    setVariationType(event.variationType);
    setVariationValue(event.variationValue);
}

std::shared_ptr<NoteOffEvent> NoteOnEvent::getNoteOff() const
{
    return noteOff;
}

void NoteOnEvent::setTrack(const int i)
{
    track = i;
    noteOff->setTrack(i);
}

void NoteOnEvent::setNote(const int i)
{
    number = std::clamp(i, 0, 127);
    noteOff->setNote(number);
}

int NoteOnEvent::getNote() const
{
    return number;
}

void NoteOnEvent::setDuration(const Duration d)
{
    if (d)
    {
        duration = std::clamp(*d, 0, 9999);
    }
}

NoteOnEvent::Duration NoteOnEvent::getDuration() const
{
    return duration;
}

void NoteOnEvent::resetDuration()
{
    duration = std::nullopt;
}

NoteOnEvent::VARIATION_TYPE NoteOnEvent::getVariationType() const
{
    return variationType;
}

void NoteOnEvent::incrementVariationType(const int amount)
{
    variationType =
        VARIATION_TYPE(std::clamp(int(variationType) + amount, 0, 3));
}

void NoteOnEvent::setVariationType(const VARIATION_TYPE type)
{
    variationType = type;
}

void NoteOnEvent::setVariationValue(const int i)
{
    if (variationType == TUNE_0)
    {
        variationValue = std::clamp(i, 0, 124);
    }
    else
    {
        variationValue = std::clamp(i, 0, 100);
    }
}

int NoteOnEvent::getVariationValue() const
{
    return variationValue;
}

void NoteOnEvent::setVelocity(const int i)
{
    velocity = std::clamp(i, 1, 127);
}

int NoteOnEvent::getVelocity() const
{
    return velocity;
}

bool NoteOnEvent::isFinalized() const
{
    return duration.has_value();
}

bool NoteOnEvent::isPlayOnly()
{
    return dynamic_cast<NoteOnEventPlayOnly *>(this) != nullptr;
}
