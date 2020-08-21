#include "LoadApsFileScreen.hpp"

#include <lcdgui/screens/LoadScreen.hpp>

using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui::screens;
using namespace std;

LoadApsFileScreen::LoadApsFileScreen(mpc::Mpc& mpc, const int layerIndex) 
	: ScreenComponent(mpc, "load-aps-file", layerIndex)
{
}

void LoadApsFileScreen::function(int i)
{
	switch (i)
	{
	case 3:
		openScreen("load");
		break;
	case 4:
	{
		auto loadScreen = dynamic_pointer_cast<LoadScreen>(mpc.screens->getScreenComponent("load"));
		apsLoader = make_unique<mpc::disk::ApsLoader>(mpc, loadScreen->getSelectedFile());
		break;
	}
	}
}
