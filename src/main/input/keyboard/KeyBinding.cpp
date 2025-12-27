#include "input/keyboard/KeyBinding.hpp"

#include "input/keyboard/KeyCodeHelper.hpp"

using namespace mpc::input::keyboard;
using namespace mpc::hardware;

ComponentId KeyBinding::getComponentId() const
{
    if (componentLabelToId.count(componentLabel) == 0)
    {
        return NONE;
    }

    return componentLabelToId.at(componentLabel);
}

int KeyBinding::getPlatformKeyCode() const
{
    return KeyCodeHelper::getPlatformFromVmpcKeyCode(keyCode);
}
