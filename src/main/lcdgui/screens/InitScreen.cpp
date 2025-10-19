#include "InitScreen.hpp"

#include <lcdgui/screens/UserScreen.hpp>
#include <lcdgui/screens/SetupScreen.hpp>

using namespace mpc::lcdgui::screens;

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
		auto userScreen = mpc.screens->get<UserScreen>();
		userScreen->resetPreferences();

		auto setupScreen = mpc.screens->get<SetupScreen>();
		setupScreen->resetPreferences();

		openScreen("sequencer");
		break;
	}
	}
}
