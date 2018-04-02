#include <sequencer/ControlChangeEvent.hpp>

using namespace mpc::sequencer;
using namespace std;

ControlChangeEvent::ControlChangeEvent()
{
}

void ControlChangeEvent::setController(int i)
{
	if (i < 0 || i > 127) return;
	controllerNumber = i;
	setChanged();
	notifyObservers(string("stepeditor"));
}

int ControlChangeEvent::getController()
{
    return controllerNumber;
}

void ControlChangeEvent::setAmount(int i)
{
    if(i < 0 || i > 127) return;

    controllerValue = i;
    setChanged();
    notifyObservers(string("stepeditor"));
}

int ControlChangeEvent::getAmount()
{
    return controllerValue;
}

void ControlChangeEvent::CopyValuesTo(std::weak_ptr<Event> dest) {
	auto lDest = dynamic_pointer_cast<ControlChangeEvent>(dest.lock());
	Event::CopyValuesTo(dest);
	lDest->setAmount(this->getAmount());
	lDest->setController(this->getController());
}
