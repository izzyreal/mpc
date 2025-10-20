#include "PopupScreen.hpp"

#include <lcdgui/Background.hpp>
#include <lcdgui/Label.hpp>

using namespace mpc::lcdgui::screens::dialog2;

PopupScreen::PopupScreen(mpc::Mpc& mpc, int layer)
	: ScreenComponent(mpc, "popup", layer)
{
	addChild(std::make_shared<Label>(mpc, "popup", "", 43, 23, 0));
	findChild<Label>("popup")->setInverted(true);
}

void PopupScreen::setText(std::string text)
{
    if (text.size() > 28)
    {
        text = text.substr(0, 28);
    }

	findChild<Label>("popup")->setText(text);
	SetDirty();
}

