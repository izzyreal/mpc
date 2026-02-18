#include "Layer.hpp"
#include "Field.hpp"

#include "ScreenComponent.hpp"

using namespace mpc::lcdgui;

Layer::Layer(const int index) : Component("layer " + std::to_string(index)) {}

Background *Layer::getBackground()
{
    return findChild<Background>("").get();
}

bool Layer::setFocus(const std::string &fieldName)
{
    const auto newFocus = findField(fieldName);

    if (!newFocus || !newFocus->isFocusable() ||
        (newFocus->IsHidden() && !newFocus->isFocusableWhenHidden()))
    {
        return false;
    }

    for (const auto &f : findFields())
    {
        f->loseFocus(fieldName);
    }

    focus = fieldName;

    newFocus->takeFocus();
    const auto screen = findChild<ScreenComponent>();
    screen->bringToFront(newFocus.get());

    if (const auto footerLabel = screen->findChild("footer-label");
        footerLabel && !footerLabel->IsHidden())
    {
        screen->bringToFront(footerLabel.get());
    }
    return true;
}

std::string Layer::getFocus()
{
    return focus;
}
