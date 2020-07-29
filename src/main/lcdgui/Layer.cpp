#include "Layer.hpp"
#include "Field.hpp"
#include "Label.hpp"

using namespace mpc::lcdgui;
using namespace std;

Layer::Layer()
	: Component("layer")
{
}

mpc::lcdgui::Background* Layer::getBackground()
{
	return dynamic_cast<Background*>(findChild<Background>("").lock().get());
}

mpc::lcdgui::FunctionKeys* Layer::getFunctionKeys()
{
	return dynamic_cast<FunctionKeys*>(findChild("function-keys").lock().get());
}

bool Layer::setFocus(string fieldName)
{
	auto newFocus = findField(fieldName).lock();

	if (!newFocus || newFocus->IsHidden() || !newFocus->isFocusable())
	{
		return false;
	}

	auto oldFocus = findField(focus).lock();

	if (oldFocus)
	{
		oldFocus->loseFocus(fieldName);
	}

	string oldFocusName = focus;

	focus = fieldName;

	newFocus->takeFocus(oldFocusName);
	bringToFront(newFocus.get());
	return true;
}

string Layer::getFocus()
{
	return focus;
}
