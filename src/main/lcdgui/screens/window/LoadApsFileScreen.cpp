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
		auto loadScreen = mpc.screens->get<LoadScreen>("load");
        auto result = mpc.getDisk().lock()->readAps2(loadScreen->getSelectedFile());
        
        if (result.has_value())
            openScreen("load");
            
        break;
	}
	}
}
