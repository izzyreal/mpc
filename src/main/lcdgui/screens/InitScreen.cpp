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
		auto userScreen = mpc.screens->get<UserScreen>("user");
		userScreen->resetPreferences();

		auto setupScreen = mpc.screens->get<SetupScreen>("setup");
		setupScreen->resetPreferences();

		openScreen("sequencer");
		break;
	}
	}
}
