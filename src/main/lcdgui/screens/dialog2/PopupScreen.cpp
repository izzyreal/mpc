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
	screenToReturnTo = "";
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

void PopupScreen::setScreenToReturnTo(const std::string& screenName)
{
	screenToReturnTo = screenName;
}

std::string PopupScreen::getScreenToReturnTo() const
{
    return screenToReturnTo;
}

PopupScreen::~PopupScreen()
{
	if (returnToScreenThread.joinable())
		returnToScreenThread.join();
}

