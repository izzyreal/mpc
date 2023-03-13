#include <engine/control/EnumControl.hpp>

#include <vector>
#include <string>
#include <stdio.h>

using namespace std;
using namespace ctoot::control;

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

nonstd::any EnumControl::getValue()
{
    return value;
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

int EnumControl::getIntValue()
{
    if (!value.has_value() || value.type() != typeid(string))
        return -1;
    
    auto valueStr = nonstd::any_cast<string>(value);
    
    auto values = getValues();
    
    for (int i = 0; i < values.size(); i++)
    {
        auto v2 = values[i];
        
        if (!v2.has_value() || v2.type() != typeid(string))
            continue;
        
        if (nonstd::any_cast<string>(v2).compare(valueStr) == 0)
            return i;
    }
    
    return -1;
}

