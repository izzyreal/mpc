#include "Control.hpp"

#include "CompoundControl.hpp"

using namespace ctoot::control;

Control::Control(int id, std::string name)
{
	this->name = name;
	this->id = id;
	annotation = name;
}

void Control::setHidden(bool h)
{
    hidden = h;
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
	
	if (c->isIndicator()) return;

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
    annotation = s;
}

void Control::setAnnotation(std::string a)
{
    annotation = a;
}

int Control::getIntValue()
{
    return -1;
}

std::string Control::getValueString()
{
	return std::string("");
}

bool Control::isHidden()
{
    return hidden;
}

bool Control::isIndicator()
{
    return indicator;
}

std::string Control::getControlPath(Control* from, std::string sep)
{
	if (parent != from) {
		if (getName().length() > 0) {
			std::string result = parent->getControlPath(from, sep) + sep + getName();
			return result;
		}
		else {
			return parent->getControlPath(from, sep);
		}
	}
	return getName();

}
