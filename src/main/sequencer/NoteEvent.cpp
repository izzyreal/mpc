#include <sequencer/NoteEvent.hpp>
#include <algorithm>
#include "assert.h"

using namespace mpc::sequencer;
using namespace mpc::engine::midi;

bool mpc::sequencer::isDrumNote(int number) { return number >= 35 && number <= 98; }

void NoteOffEvent::setNote(int i)
{
    number = i;
}

int NoteOffEvent::getNote()
{
    return number;
}

std::shared_ptr<mpc::engine::midi::ShortMessage> NoteOffEvent::createShortMessage(int channel, int transpose)
{
    auto msg = std::make_shared<ShortMessage>();
    msg->setMessage(ShortMessage::NOTE_OFF, channel, std::clamp(getNote() + transpose, 0, 127), 0);
    return msg;
}

NoteOnEvent::NoteOnEvent(int i, int vel)
{
    noteOff = std::shared_ptr<NoteOffEvent>(new NoteOffEvent());
    setNote(i);
    setVelocity(vel);
}

NoteOnEvent::NoteOnEvent(mpc::engine::midi::ShortMessage* msg) : NoteOnEvent(msg->getData1(), msg->getData2())
{
    assert(msg->getCommand() == ShortMessage::NOTE_ON);
}

NoteOnEvent::NoteOnEvent(const NoteOnEvent& event) : Event(event)
{
    noteOff = std::shared_ptr<NoteOffEvent>(new NoteOffEvent());
    setNote(event.number);
    setVelocity(event.velocity);
    setDuration(event.duration);
    setVariationType(event.variationType);
    setVariationValue(event.variationValue);
}

std::shared_ptr<mpc::engine::midi::ShortMessage> NoteOnEvent::createShortMessage(int channel, int transpose)
{
    auto msg = std::make_shared<ShortMessage>();
    msg->setMessage(ShortMessage::NOTE_ON, channel, std::clamp(getNote() + transpose, 0, 127), getVelocity());
    return msg;
}

std::shared_ptr<NoteOffEvent> NoteOnEvent::getNoteOff()
{
    return noteOff;
}

void NoteOnEvent::setTrack(int i)
{
    track = i;
    noteOff->setTrack(i);
}

void NoteOnEvent::setNote(int i)
{
    number = std::clamp(i, 0, 127);
    noteOff->setNote(number);
    notifyObservers(std::string("step-editor"));
}

int NoteOnEvent::getNote()
{
    return number;
}

void NoteOnEvent::setDuration(Duration d)
{
    if (d) duration = static_cast<int>(std::clamp(*d,0,9999));
    notifyObservers(std::string("step-editor"));
}

NoteOnEvent::Duration NoteOnEvent::getDuration()
{
    return duration;
}

void NoteOnEvent::resetDuration()
{
    duration = std::nullopt;
    notifyObservers(std::string("step-editor"));
}

NoteOnEvent::VARIATION_TYPE NoteOnEvent::getVariationType()
{
    return variationType;
}

void NoteOnEvent::incrementVariationType(int amount)
{
    variationType = VARIATION_TYPE(std::clamp(int(variationType) + amount, 0, 3));
    notifyObservers(std::string("step-editor"));
}

void NoteOnEvent::setVariationType(VARIATION_TYPE type)
{
    variationType = type;
    notifyObservers(std::string("step-editor"));
}

void NoteOnEvent::setVariationValue(int i)
{
    if (variationType == VARIATION_TYPE::TUNE_0)
    {
        variationValue = std::clamp(i, 0, 124);
    }
    else
    {
        variationValue = std::clamp(i, 0, 100);
    }
    notifyObservers(std::string("step-editor"));
}

int NoteOnEvent::getVariationValue()
{
    return variationValue;
}

void NoteOnEvent::setVelocity(int i)
{
    velocity = std::clamp(i, 1, 127);
    notifyObservers(std::string("step-editor"));
}

int NoteOnEvent::getVelocity()
{
    return velocity;
}

bool NoteOnEvent::isFinalized()
{
    return duration.has_value();
}

bool NoteOnEvent::isPlayOnly()
{
    return dynamic_cast<NoteOnEventPlayOnly*>(this) != nullptr;
}
