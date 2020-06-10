#include <sequencer/PolyPressureEvent.hpp>

using namespace mpc::sequencer;
using namespace std;

PolyPressureEvent::PolyPressureEvent()
{
}

void PolyPressureEvent::setNote(int i)
{
    if(i < 0 || i > 127) return;
    note = i;
    setChanged();
    notifyObservers(string("step-editor"));
}

int PolyPressureEvent::getNote()
{
    return note;
}

void PolyPressureEvent::setAmount(int i)
{
    if(i < 0 || i > 127)
        return;

    polyPressureValue = i;
    setChanged();
    notifyObservers(string("step-editor"));
}

int PolyPressureEvent::getAmount()
{
    return polyPressureValue;
}

void PolyPressureEvent::CopyValuesTo(std::weak_ptr<Event> dest) {
	Event::CopyValuesTo(dest);
	auto lDest = dynamic_pointer_cast<PolyPressureEvent>(dest.lock());
	lDest->setAmount(getAmount());
	lDest->setNote(getNote());
}
