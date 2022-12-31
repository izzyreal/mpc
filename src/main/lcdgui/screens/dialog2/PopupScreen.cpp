#include "PopupScreen.hpp"

#include <lcdgui/Background.hpp>
#include <lcdgui/Label.hpp>

using namespace mpc::lcdgui::screens::dialog2;

PopupScreen::PopupScreen(mpc::Mpc& mpc)
	: ScreenComponent(mpc, "popup", 3)
{
	addChild(std::make_shared<Label>(mpc, "popup", "", 43, 23, 0));
	findChild<Label>("popup")->setInverted(true);
}

void PopupScreen::close()
{
	returnToAfterInteractionScreen = "";
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

void PopupScreen::returnToScreenAfterMilliSeconds(const std::string& screenName, const int delayInMs)
{
	if (returnToScreenThread.joinable())
    {
        returnToScreenThread.join();
    }

	auto screen = screenName;
	auto layeredScreen = ls;

	returnToScreenThread = std::thread([screen, delayInMs, layeredScreen]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(delayInMs));
		layeredScreen->openScreen(screen);
	});
}

void PopupScreen::returnToScreenAfterInteraction(const std::string& screenName)
{
	returnToAfterInteractionScreen = screenName;
}

PopupScreen::~PopupScreen()
{
	if (returnToScreenThread.joinable())
		returnToScreenThread.join();
}
