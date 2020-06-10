#include <sequencer/ChannelPressureEvent.hpp>

using namespace mpc::sequencer;
using namespace std;

ChannelPressureEvent::ChannelPressureEvent()
{
}

void ChannelPressureEvent::setAmount(int i)
{
	if (i < 0 || i > 127) return;
	channelPressureValue = i;
	setChanged();
	notifyObservers(string("step-editor"));
}

int ChannelPressureEvent::getAmount()
{
    return channelPressureValue;
}

void ChannelPressureEvent::CopyValuesTo(std::weak_ptr<Event> dest) {
	auto lDest = dynamic_pointer_cast<ChannelPressureEvent>(dest.lock());
	Event::CopyValuesTo(dest);
	lDest->setAmount(this->getAmount());
}
