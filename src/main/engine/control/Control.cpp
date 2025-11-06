#include "Control.hpp"

#include "CompoundControl.hpp"

using namespace mpc::engine::control;

Control::Control(int id, const std::string &name)
{
    this->name = name;
    this->id = id;
}

int Control::getId() const
{
    return id;
}

CompoundControl *Control::getParent() const
{
    return parent;
}

void Control::setParent(CompoundControl *c)
{
    parent = c;
}

void Control::notifyParent(Control *c)
{
    derive(c);

    if (parent != nullptr)
    {
        parent->notifyParent(c);
    }
}

void Control::derive(Control *obj) {}

std::string Control::getName()
{
    return name;
}

void Control::setName(const std::string &s)
{
    name = s;
}

std::string Control::getValueString()
{
    return {};
}
