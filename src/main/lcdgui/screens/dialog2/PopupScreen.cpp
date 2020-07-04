#include "PopupScreen.hpp"

#include <lcdgui/Background.hpp>
#include <lcdgui/Label.hpp>

#include <cmath>

using namespace mpc::lcdgui::screens::dialog2;
using namespace std;

PopupScreen::PopupScreen()
	: ScreenComponent("popup", 3)
{
	addChild(make_shared<Label>("popup", "", 43, 23, 0));
	findChild<Label>("popup").lock()->setInverted(true);
}

void PopupScreen::setText(string text)
{
	findChild<Label>("popup").lock()->setText(text);
	SetDirty();
}
