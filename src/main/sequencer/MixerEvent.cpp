#include <sequencer/MixerEvent.hpp>

using namespace mpc::sequencer;

void MixerEvent::setParameter(int i)
{
	if (i < 0 || i > 3) return;
	mixerParameter = i;
	
	notifyObservers(std::string("step-editor"));
}

int MixerEvent::getParameter() const
{
    return mixerParameter;
}

void MixerEvent::setPadNumber(int i)
{
    if(i < 0 || i > 63) return;

    padNumber = i;
    
    notifyObservers(std::string("step-editor"));
}

int MixerEvent::getPad() const
{
    return padNumber;
}

void MixerEvent::setValue(int i)
{
    if(i < 0 || i > 100) return;

    mixerParameterValue = i;
    
    notifyObservers(std::string("step-editor"));
}

int MixerEvent::getValue() const
{
    return mixerParameterValue;
}

mpc::sequencer::MixerEvent::MixerEvent(const MixerEvent& event) : Event(event)
{
    setPadNumber(event.getPad());
    setParameter(event.getParameter());
    setValue(event.getValue());
}

void MixerEvent::CopyValuesTo(std::weak_ptr<Event> dest) {
	Event::CopyValuesTo(dest);
	auto lDest = std::dynamic_pointer_cast<MixerEvent>(dest.lock());
	lDest->setPadNumber(getPad());
	lDest->setParameter(getParameter());
	lDest->setValue(getValue());
}
