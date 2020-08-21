#include "InitScreen.hpp"

#include <lcdgui/screens/UserScreen.hpp>
#include <lcdgui/screens/SetupScreen.hpp>

using namespace mpc::lcdgui::screens;
using namespace std;

InitScreen::InitScreen(mpc::Mpc& mpc, const int layerIndex) 
	: ScreenComponent(mpc, "init", layerIndex)
{
}

void InitScreen::function(int i)
{
	init();
	
	switch (i)
	{
	case 0:
		openScreen("others");
		break;
	case 2:
		openScreen("ver");
		break;
	case 5:
	{
		auto userScreen = dynamic_pointer_cast<UserScreen>(mpc.screens->getScreenComponent("user"));
		userScreen->resetPreferences();

		auto setupScreen = dynamic_pointer_cast<SetupScreen>(mpc.screens->getScreenComponent("setup"));
		setupScreen->resetPreferences();

		openScreen("sequencer");
		break;
	}
	}
}
