#include <engine/control/EnumControl.hpp>

#include <vector>
#include <string>
#include <stdio.h>

using namespace std;
using namespace mpc::engine::control;

using namespace nonstd;

EnumControl::EnumControl(int id, string name, nonstd::any value) : Control(id, name)
{
    this->value = value;
}

void EnumControl::setValue(nonstd::any value)
{
    this->value = value;
    notifyParent(this);
}

string EnumControl::getValueString()
{
    if (!value.has_value())
        return nullptr;
    
    if (value.type() == typeid(int))
        return to_string(nonstd::any_cast<int>(value));
    else if (value.type() == typeid(string))
        return nonstd::any_cast<string>(value);
    
    return nullptr;
}


