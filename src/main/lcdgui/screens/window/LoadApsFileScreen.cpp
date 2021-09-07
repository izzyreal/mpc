#include "LoadApsFileScreen.hpp"

#include <disk/AbstractDisk.hpp>
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
        std::function<void()> on_success = [&]() { openScreen("load"); };
		auto loadScreen = mpc.screens->get<LoadScreen>("load");
        mpc.getDisk().lock()->readAps2(loadScreen->getSelectedFile(), on_success);
        break;
	}
	}
}
