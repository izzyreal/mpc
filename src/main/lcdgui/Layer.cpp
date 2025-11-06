#include "Layer.hpp"
#include "Field.hpp"

#include "ScreenComponent.hpp"

using namespace mpc::lcdgui;

Layer::Layer(const int index) : Component("layer " + std::to_string(index)) {}

Background *Layer::getBackground()
{
    return findChild<Background>("").get();
}

FunctionKeys *Layer::getFunctionKeys()
{
    return findChild<FunctionKeys>("function-keys").get();
}

bool Layer::setFocus(const std::string &fieldName)
{
    const auto newFocus = findField(fieldName);

    if (!newFocus || newFocus->IsHidden() || !newFocus->isFocusable())
    {
        return false;
    }

    for (const auto &f : findFields())
    {
        f->loseFocus(fieldName);
    }

    focus = fieldName;

    newFocus->takeFocus();
    findChild<ScreenComponent>()->bringToFront(newFocus.get());
    return true;
}

std::string Layer::getFocus()
{
    return focus;
}
