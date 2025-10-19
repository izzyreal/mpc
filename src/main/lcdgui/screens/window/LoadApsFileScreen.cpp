#include "LoadApsFileScreen.hpp"

#include <disk/AbstractDisk.hpp>
#include <lcdgui/screens/LoadScreen.hpp>

using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui::screens;

LoadApsFileScreen::LoadApsFileScreen(mpc::Mpc& mpc, const int layerIndex) 
	: ScreenComponent(mpc, "load-aps-file", layerIndex)
{
}

void LoadApsFileScreen::function(int i)
{
	switch (i)
	{
	case 3:
        mpc.getLayeredScreen()->openScreen<LoadScreen>();
		break;
	case 4:
	{
        std::function<void()> on_success = [&]() { mpc.getLayeredScreen()->closeWindow(); mpc.getLayeredScreen()->closeWindow(); };
		auto loadScreen = mpc.screens->get<LoadScreen>();
        mpc.getDisk()->readAps2(loadScreen->getSelectedFile(), on_success);
        break;
	}
	}
}
