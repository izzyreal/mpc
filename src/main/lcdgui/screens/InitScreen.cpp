#include "InitScreen.hpp"

#include <lcdgui/screens/UserScreen.hpp>
#include <lcdgui/screens/SetupScreen.hpp>

using namespace mpc::lcdgui::screens;
using namespace std;

InitScreen::InitScreen(const int layerIndex) 
	: ScreenComponent("init", layerIndex)
{
}

void InitScreen::function(int i)
{
	init();
	
	switch (i)
	{
	case 0:
		ls.lock()->openScreen("others");
		break;
	case 2:
		ls.lock()->openScreen("ver");
		break;
	case 5:
	{
		auto userScreen = dynamic_pointer_cast<UserScreen>(Screens::getScreenComponent("user"));
		userScreen->resetPreferences();

		auto setupScreen = dynamic_pointer_cast<SetupScreen>(Screens::getScreenComponent("setup"));
		setupScreen->resetPreferences();

		ls.lock()->openScreen("sequencer");
		break;
	}
	}
}
