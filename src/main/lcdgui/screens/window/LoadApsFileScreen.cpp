#include "LoadApsFileScreen.hpp"

#include <lcdgui/screens/LoadScreen.hpp>
#include <lcdgui/screens/dialog2/PopupScreen.hpp>

using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui::screens::dialog2;
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
		try
		{
			apsLoader = make_unique<mpc::disk::ApsLoader>(mpc, loadScreen->getSelectedFile());
		}
		catch (const exception& e)
		{
			auto popupScreen = mpc.screens->get<PopupScreen>("popup");
			popupScreen->setText("Wrong file format");
			popupScreen->returnToScreenAfterInteraction("load");
			mpc.getLayeredScreen().lock()->openScreen("popup");
		}
		break;
	}
	}
}
