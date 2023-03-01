#include <sequencer/NoteEvent.hpp>
#include <algorithm>


using namespace mpc::sequencer;
using namespace ctoot::midi::core;

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
    if(i < 0 || i > 3) return;

    variationTypeNumber = i;
    
    notifyObservers(std::string("step-editor"));
}

void NoteEvent::setVariationValue(int i)
{
    if(i < 0 || i > 124) return;

    if(variationTypeNumber != 0 && i > 100) i = 100;

    variationValue = i;
    
    notifyObservers(std::string("step-editor"));
}

int NoteEvent::getVariationValue()
{
    return variationValue;
}

void NoteEvent::setVelocity(int i)
{
    if(i < 1 || i > 127) return;

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

std::shared_ptr<ShortMessage> NoteEvent::createShortMessage(int channel, int transpose)
{
    auto msg = std::make_shared<ctoot::midi::core::ShortMessage>();
    msg->setMessage(getVelocity() == 0 ? ShortMessage::NOTE_OFF : ShortMessage::NOTE_ON, channel, std::clamp(getNote()+transpose,0,127), getVelocity());
    return msg;
}