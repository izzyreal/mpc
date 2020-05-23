#include <lcdgui/screens/PurgeScreen.hpp>

#include <sampler/Sampler.hpp>

using namespace mpc::lcdgui::screens;
using namespace std;

PurgeScreen::PurgeScreen(const int layerIndex) 
	: ScreenComponent("purge", layerIndex)
{
}

void PurgeScreen::function(int f)
{
	init();
	
	switch (f)
	{
	case 0:
		ls.lock()->openScreen("programassign");
		break;
	case 1:
		ls.lock()->openScreen("programparams");
		break;
	case 2:
		ls.lock()->openScreen("drum");
		break;
	case 3:
		ls.lock()->openScreen("selectdrum");
		break;
	case 4:
		sampler.lock()->purge();
		ls.lock()->openScreen("purge");
		break;
	}
}
