#include "Layer.hpp"
#include "Field.hpp"
#include "Label.hpp"

using namespace mpc::lcdgui;

Layer::Layer() : Component("layer")
{
}

mpc::lcdgui::Background* Layer::getBackground()
{
	return dynamic_cast<Background*>(findChild<Background>("").lock().get());
}

mpc::lcdgui::FunctionKeys* Layer::getFunctionKeys()
{
	return findChild<FunctionKeys>("function-keys").lock().get();
}

bool Layer::setFocus(std::string fieldName)
{
	auto newFocus = findField(fieldName).lock();

	if (!newFocus || newFocus->IsHidden() || !newFocus->isFocusable())
		return false;

	auto oldFocus = findField(focus).lock();

	if (oldFocus)
		oldFocus->loseFocus(fieldName);

	focus = fieldName;

    newFocus->takeFocus();
	bringToFront(newFocus.get());
	return true;
}

std::string Layer::getFocus()
{
	return focus;
}
