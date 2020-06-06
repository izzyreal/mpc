#include "InitScreen.hpp"

#include <ui/UserDefaults.hpp>

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
		mpc::ui::UserDefaults::instance().initialize();
		ls.lock()->openScreen("sequencer");
		break;
	}
}
