#include "Layer.hpp"
#include "Field.hpp"
#include "Label.hpp"

using namespace mpc::lcdgui;
using namespace std;

Layer::Layer()
	: Component("layer")
{
}

mpc::lcdgui::Background* Layer::getBackground() {
	return dynamic_cast<Background*>(findChild("background").lock().get());
}

mpc::lcdgui::FunctionKeys* Layer::getFunctionKeys() {
	return dynamic_cast<FunctionKeys*>(findChild("function-keys").lock().get());
}

void Layer::setFocus(string textFieldName) {

	string oldFocus = focus;
	// First make sure the desired focus field exists and is focusable
	bool exists = false;
	for (auto& a : findFields()) {
		auto tf = dynamic_pointer_cast<Field>(a.lock());
		//if (!tf->IsHidden() && tf->getName().compare(textFieldName) == 0 && tf->isFocusable()) {
		if (tf->getName().compare(textFieldName) == 0 && tf->isFocusable()) {
			exists = true;
			break;
		}
	}

	if (!exists) {
		return;
	}

	for (auto& a : findFields()) {
		auto tf = dynamic_pointer_cast<Field>(a.lock());
		if (tf->getName().compare(focus) == 0 && tf->isFocusable()) {
			findParameter(tf->getName()).lock()->loseFocus();
			break;
		}
	}

	focus = textFieldName;

	for (auto& a : findFields()) {
		auto tf = dynamic_pointer_cast<Field>(a.lock());
		if (tf->getName().compare(textFieldName) == 0 && tf->isFocusable()) {
			findParameter(tf->getName()).lock()->takeFocus();
			break;
		}
	}
}

string Layer::getFocus() {
	return focus;
}
