#include "file/mid/event/Controller.hpp"

#include "file/mid/event/ChannelEvent.hpp"

using namespace mpc::file::mid::event;

Controller::Controller(int tick, int channel, int controllerType, int value)
    : ChannelEvent(tick, ChannelEvent::CONTROLLER, channel, controllerType,
                   value)
{
}

Controller::Controller(int tick, int delta, int channel, int controllerType,
                       int value)
    : ChannelEvent(tick, delta, ChannelEvent::CONTROLLER, channel,
                   controllerType, value)
{
}

int Controller::getControllerType() const
{
    return mValue1;
}

int Controller::getValue() const
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
