#include "PopupScreen.hpp"

#include <lcdgui/Background.hpp>
#include <lcdgui/Label.hpp>

#include <cmath>

using namespace mpc::lcdgui::screens::dialog2;
using namespace std;

PopupScreen::PopupScreen(mpc::Mpc& mpc)
	: ScreenComponent(mpc, "popup", 3)
{
	addChild(make_shared<Label>(mpc, "popup", "", 43, 23, 0));
	findChild<Label>("popup").lock()->setInverted(true);
}

void PopupScreen::setText(string text)
{
	findChild<Label>("popup").lock()->setText(text);
	SetDirty();
}

void PopupScreen::returnToScreenAfterMilliSeconds(const std::string& screenName, const int delayInMs)
{
	if (returnToScreenThread.joinable())
		returnToScreenThread.join();

	auto screen = screenName;
	auto layeredScreen = ls.lock();

	returnToScreenThread = thread([screen, delayInMs, layeredScreen]() {
		this_thread::sleep_for(chrono::milliseconds(delayInMs));
		layeredScreen->openScreen(screen);
	});
}

PopupScreen::~PopupScreen()
{
	if (returnToScreenThread.joinable())
		returnToScreenThread.join();
}
