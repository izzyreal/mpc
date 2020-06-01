#include "LoadApsFileScreen.hpp"

#include <ui/disk/DiskGui.hpp>

#include <lcdgui/screens/LoadScreen.hpp>

using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui::screens;
using namespace std;

LoadApsFileScreen::LoadApsFileScreen(const int layerIndex) 
	: ScreenComponent("load-aps-file", layerIndex)
{
}

void LoadApsFileScreen::function(int i)
{
	switch (i)
	{
	case 3:
		ls.lock()->openScreen("load");
		break;
	case 4:
	{
		auto loadScreen = dynamic_pointer_cast<LoadScreen>(Screens::getScreenComponent("load"));
		apsLoader = make_unique<mpc::disk::ApsLoader>(loadScreen->getSelectedFile());
		break;
	}
	}
}
