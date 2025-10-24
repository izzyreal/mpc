#include "midi/event/Controller.hpp"

#include "midi/event/ChannelEvent.hpp"

using namespace mpc::midi::event;

Controller::Controller(int tick, int channel, int controllerType, int value) : ChannelEvent(tick, ChannelEvent::CONTROLLER, channel, controllerType, value)
{
}

Controller::Controller(int tick, int delta, int channel, int controllerType, int value) : ChannelEvent(tick, delta, ChannelEvent::CONTROLLER, channel, controllerType, value)
{
}

int Controller::getControllerType()
{
    return mValue1;
}

int Controller::getValue()
{
    return mValue2;
}

void Controller::setControllerType(int t)
{
    mValue1 = t;
}

void Controller::setValue(int v)
{
    mValue2 = v;
}
