#include <sequencer/SystemExclusiveEvent.hpp>

using namespace mpc::sequencer;
using namespace std;

SystemExclusiveEvent::SystemExclusiveEvent()
{
	bytes = vector<char>(2);
}

void SystemExclusiveEvent::setByteA(int i)
{
    if(i < 0 || i > 255) return;

    bytes[0] = i;
    setChanged();
    notifyObservers(string("stepeditor"));
}

int SystemExclusiveEvent::getByteA()
{
    return bytes[0];
}

void SystemExclusiveEvent::setByteB(int i)
{
    if(i < 0 || i > 255) return;

	bytes[1] = i;
    setChanged();
    notifyObservers(string("stepeditor"));
}

int SystemExclusiveEvent::getByteB()
{
    return bytes[1];
}

void SystemExclusiveEvent::setBytes(vector<char>* ba)
{
    bytes = *ba;
}

vector<char>* SystemExclusiveEvent::getBytes()
{
    return &bytes;
}

void SystemExclusiveEvent::CopyValuesTo(std::weak_ptr<Event> dest) {
	Event::CopyValuesTo(dest);
	auto lDest = dynamic_pointer_cast<SystemExclusiveEvent>(dest.lock());
	lDest->setBytes(getBytes());
	lDest->setByteA(getByteA());
	lDest->setByteB(getByteB());
}
