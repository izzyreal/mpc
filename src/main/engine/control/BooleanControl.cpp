#include "engine/control/BooleanControl.hpp"

using namespace mpc::engine::control;
using namespace std;

BooleanControl::BooleanControl(int id, string name) : Control(id, name) {}

void BooleanControl::setValue(bool value)
{
    if (value != this->value)
    {
        this->value = value;
        notifyParent(this);
    }
}

bool BooleanControl::getValue()
{
    return value;
}
