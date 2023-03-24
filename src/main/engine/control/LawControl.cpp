#include <engine/control/LawControl.hpp>
#include <engine/control/ControlLaw.hpp>

using namespace mpc::engine::control;
using namespace std;

LawControl::LawControl(int id, string name, shared_ptr<ControlLaw> law, float precision, float initialValue)
	: Control(id, name)
{
	this->law = law;
	this->precision = precision;
	this->value = initialValue;
}

float LawControl::getValue()
{
    return value;
}

string LawControl::getValueString()
{
	string res1 = law->getUnits();
	string res2 = to_string(getValue());
	return string(res1 + res2);
}

void LawControl::setValue(float value)
{
	this->value = value;
	notifyParent(this);
}
