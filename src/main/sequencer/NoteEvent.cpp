#include <sequencer/NoteEvent.hpp>

using namespace mpc::sequencer;
using namespace std;

NoteEvent::NoteEvent()
	: NoteEvent(60)
{
}

NoteEvent::NoteEvent(int i) 
{
	number = i;
	noteOff = make_shared<NoteEvent>(false);
}

NoteEvent::NoteEvent(bool noteOffTrue) 
{
	// noteoff ctor should not create a noteoff
}

weak_ptr<NoteEvent> NoteEvent::getNoteOff()
{
    return noteOff;
}

void NoteEvent::setNote(int i)
{
    if(i < 0) return;

    if(i > 127 && i != 256) return;

    if(number == i) return;

    number = i;
    
    notifyObservers(string("step-editor"));
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
	
	notifyObservers(string("step-editor"));
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
    
    notifyObservers(string("step-editor"));
}

void NoteEvent::setVariationValue(int i)
{
    if(i < 0 || i > 128) return;

    if(variationTypeNumber != 0 && i > 100) i = 100;

    variationValue = i;
    
    notifyObservers(string("step-editor"));
}

int NoteEvent::getVariationValue()
{
    return variationValue;
}

void NoteEvent::setVelocity(int i)
{
    if(i < 1 || i > 127) return;

    velocity = i;
    
    notifyObservers(string("step-editor"));
}

void NoteEvent::setVelocityZero()
{
    velocity = 0;
}

int NoteEvent::getVelocity()
{
    return velocity;
}

void NoteEvent::CopyValuesTo(weak_ptr<Event> dest) {
	Event::CopyValuesTo(dest);
	auto lDest = dynamic_pointer_cast<NoteEvent>(dest.lock());
	lDest->setVariationTypeNumber(getVariationType());
	lDest->setVariationValue(getVariationValue());
	lDest->setNote(getNote());
	lDest->setVelocity(getVelocity());
	lDest->setDuration(getDuration());
}

NoteEvent::~NoteEvent() {
}
