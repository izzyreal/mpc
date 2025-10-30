#include "sequencer/MixerEvent.hpp"

using namespace mpc::sequencer;

void MixerEvent::setParameter(int i)
{
    if (i < 0 || i > 3)
    {
        return;
    }
    mixerParameter = i;

}

int MixerEvent::getParameter() const
{
    return mixerParameter;
}

void MixerEvent::setPadNumber(int i)
{
    if (i < 0 || i > 63)
    {
        return;
    }

    padNumber = i;

}

int MixerEvent::getPad() const
{
    return padNumber;
}

void MixerEvent::setValue(int i)
{
    if (i < 0 || i > 100)
    {
        return;
    }

    mixerParameterValue = i;

}

int MixerEvent::getValue() const
{
    return mixerParameterValue;
}

mpc::sequencer::MixerEvent::MixerEvent(const MixerEvent &event) : Event(event)
{
    setPadNumber(event.getPad());
    setParameter(event.getParameter());
    setValue(event.getValue());
}
