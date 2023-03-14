#include <sequencer/NoteEvent.hpp>
#include <algorithm>

using namespace mpc::sequencer;

//------------
void mpc::sequencer::NoteOffEvent::setNote(int i)
{
    number = i;
}

int mpc::sequencer::NoteOffEvent::getNote()
{
    return number;
}
//-------------
mpc::sequencer::NoteOnEvent::NoteOnEvent(int i)
{
    noteOff = std::shared_ptr<NoteOffEvent>(new NoteOffEvent());
    setNote(i);
    
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

void mpc::sequencer::NoteOnEvent::setVariationType(VARIATION_TYPE type)
{
    variationType = type;
    notifyObservers(std::string("step-editor"));
}

void mpc::sequencer::NoteOnEvent::setVariationValue(int i)
{
    if (variationType == VARIATION_TYPE::TUNE)
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






NoteEvent::NoteEvent()
    : NoteEvent(60)
{
}

NoteEvent::NoteEvent(int i)
{
    number = i;
    noteOff = std::make_shared<NoteEvent>(true, 0);
    noteOff->number = number;
}

NoteEvent::NoteEvent(bool /*dummyParameter*/, int /* noteOnTick */)
{
    // noteoff ctor should not create a noteoff
}

std::shared_ptr<NoteEvent> NoteEvent::getNoteOff()
{
    return noteOff;
}

void NoteEvent::setNote(int i)
{
    if (i < 0) return;

    if (i > 127 && i != 256) return;

    if (number == i) return;

    number = i;

    noteOff->number = number;

    notifyObservers(std::string("step-editor"));
}

int NoteEvent::getNote()
{
    return number;
}

void NoteEvent::setDuration(int i)
{
    if (i < 0 || i > 9999)
    {
        return;
    }
    duration = i;

    notifyObservers(std::string("step-editor"));
}

int NoteEvent::getDuration()
{
    return duration;
}

int NoteEvent::getVariationType()
{
    return variationTypeNumber;
}

void NoteEvent::setVariationTypeNumber(int i)
{
    if (i < 0 || i > 3) return;

    variationTypeNumber = i;

    notifyObservers(std::string("step-editor"));
}

void NoteEvent::setVariationValue(int i)
{
    if (i < 0 || i > 128) return;

    if (variationTypeNumber != 0 && i > 100) i = 100;

    variationValue = i;

    notifyObservers(std::string("step-editor"));
}

int NoteEvent::getVariationValue()
{
    return variationValue;
}

void NoteEvent::setVelocity(int i)
{
    if (i < 1 || i > 127) return;

    velocity = i;

    notifyObservers(std::string("step-editor"));
}

void NoteEvent::setVelocityZero()
{
    velocity = 0;
}

int NoteEvent::getVelocity()
{
    return velocity;
}

void NoteEvent::CopyValuesTo(std::weak_ptr<Event> dest)
{
    Event::CopyValuesTo(dest);
    auto lDest = std::dynamic_pointer_cast<NoteEvent>(dest.lock());
    lDest->setVariationTypeNumber(getVariationType());
    lDest->setVariationValue(getVariationValue());
    lDest->setNote(getNote());
    lDest->velocity = velocity;
    lDest->setDuration(getDuration());
}