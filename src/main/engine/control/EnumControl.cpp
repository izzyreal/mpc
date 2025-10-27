#include "engine/control/EnumControl.hpp"

#include <utility>
#include <vector>
#include <string>

using namespace mpc::engine::control;

EnumControl::EnumControl(int id, std::string name,
                         const std::variant<int, std::string> &valueToUse)
    : Control(id, std::move(name)), value(valueToUse)
{
}

void EnumControl::setValue(const std::variant<int, std::string> &valueToUse)
{
    value = valueToUse;
    notifyParent(this);
}

std::string EnumControl::getValueString()
{
    if (auto intValue = std::get_if<int>(&value))
    {
        return std::to_string(*intValue);
    }
    else if (auto strValue = std::get_if<std::string>(&value))
    {
        return *strValue;
    }

    return {};
}
