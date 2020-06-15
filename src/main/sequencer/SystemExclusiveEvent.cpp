#include <sequencer/SystemExclusiveEvent.hpp>

using namespace mpc::sequencer;
using namespace std;

void SystemExclusiveEvent::setByteA(unsigned char i)
{
    if (i < 0 || i > 255)
    {
        return;
    }

    bytes[0] = i;
    
    notifyObservers(string("step-editor"));
}

unsigned char SystemExclusiveEvent::getByteA()
{
    return bytes[0];
}

void SystemExclusiveEvent::setByteB(unsigned char i)
{
    if (i < 0 || i > 255)
    {
        return;
    }

    bytes[1] = i;
    
    notifyObservers(string("step-editor"));
}

unsigned char SystemExclusiveEvent::getByteB()
{
    return bytes[1];
}

void SystemExclusiveEvent::setBytes(const vector<unsigned char>& ba)
{
    bytes = ba;
}

vector<unsigned char>& SystemExclusiveEvent::getBytes()
{
    return bytes;
}

void SystemExclusiveEvent::CopyValuesTo(std::weak_ptr<Event> dest)
{
	Event::CopyValuesTo(dest);
	auto lDest = dynamic_pointer_cast<SystemExclusiveEvent>(dest.lock());
	lDest->setBytes(getBytes());
}
