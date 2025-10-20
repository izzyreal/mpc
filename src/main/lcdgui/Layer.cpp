#include "Layer.hpp"
#include "Field.hpp"
#include "Label.hpp"

#include "ScreenComponent.hpp"

using namespace mpc::lcdgui;

Layer::Layer(int index) : Component("layer " + std::to_string(index))
{
}

mpc::lcdgui::Background* Layer::getBackground()
{
	return findChild<Background>("").get();
}

mpc::lcdgui::FunctionKeys* Layer::getFunctionKeys()
{
	return findChild<FunctionKeys>("function-keys").get();
}

bool Layer::setFocus(std::string fieldName)
{
	auto newFocus = findField(fieldName);

	if (!newFocus || newFocus->IsHidden() || !newFocus->isFocusable())
		return false;

	auto oldFocus = findField(focus);

	if (oldFocus)
		oldFocus->loseFocus(fieldName);

	focus = fieldName;

    newFocus->takeFocus();
	findChild<ScreenComponent>()->bringToFront(newFocus.get());
	return true;
}

std::string Layer::getFocus()
{
	return focus;
}
