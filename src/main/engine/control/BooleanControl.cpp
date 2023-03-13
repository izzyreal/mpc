#include <engine/control/BooleanControl.hpp>

using namespace ctoot::control;
using namespace std;

BooleanControl::BooleanControl(int id, string name, bool initialValue) : Control(id, name)
{
}

void BooleanControl::setValue(bool value)
{
	if (value != this->value) {
		this->value = value;
		notifyParent(this);
	}
}

bool BooleanControl::getValue()
{
    return value;
}

int BooleanControl::getIntValue()
{
	return getValue() ? 1 : 0;
}

