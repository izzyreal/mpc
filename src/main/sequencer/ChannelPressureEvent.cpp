#include <sequencer/ChannelPressureEvent.hpp>

using namespace mpc::sequencer;

void ChannelPressureEvent::setAmount(int i)
{
	if (i < 0 || i > 127) return;
	channelPressureValue = i;
	
	notifyObservers(std::string("step-editor"));
}

int ChannelPressureEvent::getAmount()
{
    return channelPressureValue;
}

void ChannelPressureEvent::CopyValuesTo(std::weak_ptr<Event> dest) {
	auto lDest = std::dynamic_pointer_cast<ChannelPressureEvent>(dest.lock());
	Event::CopyValuesTo(dest);
	lDest->setAmount(this->getAmount());
}
