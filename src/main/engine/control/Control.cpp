#include "Control.hpp"

#include "CompoundControl.hpp"

using namespace mpc::engine::control;

Control::Control(int id, std::string name)
{
	this->name = name;
	this->id = id;
}

int Control::getId()
{
    return id;
}


CompoundControl* Control::getParent()
{
    return parent;
}

void Control::setParent(CompoundControl* c)
{
	parent = c;
}

void Control::notifyParent(Control* c)
{
	derive(c);
	
	if (parent != nullptr) {
		parent->notifyParent(c);
	}
}

void Control::derive(Control* obj)
{
}

std::string Control::getName()
{
    return name;
}

void Control::setName(std::string s)
{
    name = s;
}

std::string Control::getValueString()
{
	return std::string("");
}
