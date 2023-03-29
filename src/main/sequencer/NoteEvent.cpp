#include <sequencer/NoteEvent.hpp>
#include <algorithm>

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
//-------------
mpc::sequencer::NoteOnEvent::NoteOnEvent(int i, int vel)
{
    noteOff = std::shared_ptr<NoteOffEvent>(new NoteOffEvent());
    setNote(i);
    setVelocity(vel);
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

void mpc::sequencer::NoteOnEvent::setDuration(int i)
{
    duration = std::clamp(i,0,9999);
    notifyObservers(std::string("step-editor"));
}

int mpc::sequencer::NoteOnEvent::getDuration()
{
    return duration;
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
    velocity = std::clamp(i, 1, 127);;
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
    return duration != -1;
}

OldNoteEvent::OldNoteEvent()
    : OldNoteEvent(60)
{
}

OldNoteEvent::OldNoteEvent(int i)
{
    number = i;
    noteOff = std::make_shared<OldNoteEvent>(true, 0);
    noteOff->number = number;
}

OldNoteEvent::OldNoteEvent(bool /*dummyParameter*/, int /* noteOnTick */)
{
    // noteoff ctor should not create a noteoff
}

std::shared_ptr<OldNoteEvent> OldNoteEvent::getNoteOff()
{
    return noteOff;
}

void OldNoteEvent::setNote(int i)
{
    if (i < 0) return;

    if (i > 127 && i != 256) return;

    if (number == i) return;

    number = i;

    noteOff->number = number;

    notifyObservers(std::string("step-editor"));
}

int OldNoteEvent::getNote()
{
    return number;
}

void OldNoteEvent::setDuration(int i)
{
    if (i < 0 || i > 9999)
    {
        return;
    }
    duration = i;

    notifyObservers(std::string("step-editor"));
}

int OldNoteEvent::getDuration()
{
    return duration;
}

int OldNoteEvent::getVariationType()
{
    return variationTypeNumber;
}

void OldNoteEvent::setVariationTypeNumber(int i)
{
    if (i < 0 || i > 3) return;

    variationTypeNumber = i;

    notifyObservers(std::string("step-editor"));
}

void OldNoteEvent::setVariationValue(int i)
{
    if (i < 0 || i > 128) return;

    if (variationTypeNumber != 0 && i > 100) i = 100;

    variationValue = i;

    notifyObservers(std::string("step-editor"));
}

int OldNoteEvent::getVariationValue()
{
    return variationValue;
}

void OldNoteEvent::setVelocity(int i)
{
    if (i < 1 || i > 127) return;

    velocity = i;

    notifyObservers(std::string("step-editor"));
}

void OldNoteEvent::setVelocityZero()
{
    velocity = 0;
}

int OldNoteEvent::getVelocity()
{
    return velocity;
}

bool mpc::sequencer::OldNoteEvent::isDrumNote()
{
    return number >= 35 && number <= 98;
}

void OldNoteEvent::CopyValuesTo(std::weak_ptr<Event> dest)
{
    Event::CopyValuesTo(dest);
    auto lDest = std::dynamic_pointer_cast<OldNoteEvent>(dest.lock());
    lDest->setVariationTypeNumber(getVariationType());
    lDest->setVariationValue(getVariationValue());
    lDest->setNote(getNote());
    lDest->velocity = velocity;

	lDest->setDuration(getDuration());
}

std::shared_ptr<ShortMessage> OldNoteEvent::createShortMessage(int channel, int transpose)
{
    auto msg = std::make_shared<ShortMessage>();
    msg->setMessage(getVelocity() == 0 ? ShortMessage::NOTE_OFF : ShortMessage::NOTE_ON, channel, std::clamp(getNote()+transpose,0,127), getVelocity());
    return msg;

}