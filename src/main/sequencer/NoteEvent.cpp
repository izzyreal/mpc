#include <sequencer/NoteEvent.hpp>
#include <algorithm>
#include "assert.h"

using namespace mpc::sequencer;
using namespace mpc::engine::midi;

//------------
void mpc::sequencer::NoteOffEvent::setNote(int i)
{
    number = i;
}

int mpc::sequencer::NoteOffEvent::getNote()
{
    return number;
}
bool mpc::sequencer::NoteOffEvent::isDrumNote()
{
    return number >= 35 && number <= 98;
}
std::shared_ptr<mpc::engine::midi::ShortMessage> mpc::sequencer::NoteOffEvent::createShortMessage(int channel, int transpose)
{
    auto msg = std::make_shared<ShortMessage>();
    msg->setMessage(ShortMessage::NOTE_OFF, channel, std::clamp(getNote() + transpose, 0, 127), 0);
    return msg;
}

void NoteOnEvent::CopyValuesTo(std::weak_ptr<Event> dest)
{
    auto lDest = std::dynamic_pointer_cast<NoteOnEvent>(dest.lock());
    Event::CopyValuesTo(dest);
    lDest->setVariationType(getVariationType());
    lDest->setVariationValue(getVariationValue());
    lDest->setNote(getNote());
    lDest->setVelocity(getVelocity());
    lDest->setDuration(getDuration());
}

//-------------
mpc::sequencer::NoteOnEvent::NoteOnEvent(int i, int vel)
{
    noteOff = std::shared_ptr<NoteOffEvent>(new NoteOffEvent());
    setNote(i);
    setVelocity(vel);
}

mpc::sequencer::NoteOnEvent::NoteOnEvent(mpc::engine::midi::ShortMessage* msg) : NoteOnEvent(msg->getData1(), msg->getData2())
{
    assert(msg->getCommand() == ShortMessage::NOTE_ON);
}

std::shared_ptr<mpc::engine::midi::ShortMessage> mpc::sequencer::NoteOnEvent::createShortMessage(int channel, int transpose)
{
    auto msg = std::make_shared<ShortMessage>();
    msg->setMessage(ShortMessage::NOTE_ON, channel, std::clamp(getNote() + transpose, 0, 127), getVelocity());
    return msg;
}

std::shared_ptr<NoteOffEvent> mpc::sequencer::NoteOnEvent::getNoteOff()
{
    return noteOff;
}

void mpc::sequencer::NoteOnEvent::setTrack(int i)
{
    track = i;
    noteOff->setTrack(i);
}

void mpc::sequencer::NoteOnEvent::setNote(int i)
{
    number = std::clamp(i, 0, 127);
    noteOff->setNote(number);
    notifyObservers(std::string("step-editor"));
}

int mpc::sequencer::NoteOnEvent::getNote()
{
    return number;
}

void mpc::sequencer::NoteOnEvent::setDuration(Duration d)
{
    assert(d != -1);
    duration = std::clamp(d.value(),0,9999);
    notifyObservers(std::string("step-editor"));
}

mpc::sequencer::NoteOnEvent::Duration mpc::sequencer::NoteOnEvent::getDuration()
{
    return duration;
}

void mpc::sequencer::NoteOnEvent::resetDuration()
{
    duration = std::nullopt;
    notifyObservers(std::string("step-editor"));
}

mpc::sequencer::NoteOnEvent::VARIATION_TYPE mpc::sequencer::NoteOnEvent::getVariationType()
{
    return variationType;
}

void mpc::sequencer::NoteOnEvent::incrementVariationType(int amount)
{
    variationType = VARIATION_TYPE(std::clamp(int(variationType) + amount, 0, 3));
    notifyObservers(std::string("step-editor"));
}

void mpc::sequencer::NoteOnEvent::setVariationType(VARIATION_TYPE type)
{
    variationType = type;
    notifyObservers(std::string("step-editor"));
}

void mpc::sequencer::NoteOnEvent::setVariationValue(int i)
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

int mpc::sequencer::NoteOnEvent::getVariationValue()
{
    return variationValue;
}

void mpc::sequencer::NoteOnEvent::setVelocity(int i)
{
    velocity = std::clamp(i, 1, 127);
    notifyObservers(std::string("step-editor"));
}

int mpc::sequencer::NoteOnEvent::getVelocity()
{
    return velocity;
}

bool mpc::sequencer::NoteOnEvent::isDrumNote()
{
    return number >= 35 && number <= 98;
}

bool mpc::sequencer::NoteOnEvent::isFinalized()
{
    return duration.has_value();
}