#include "DeleteAllSoundScreen.hpp"

using namespace mpc::lcdgui::screens::dialog;
using namespace std;

DeleteAllSoundScreen::DeleteAllSoundScreen(const int layerIndex)
	: ScreenComponent("delete-all-sound", layerIndex)
{
}

void DeleteAllSoundScreen::function(int i)
{
	init();
	
	switch (i)
	{
	case int(3) :
		ls.lock()->openScreen("deletesound");
		break;
	case 4:
		sampler.lock()->deleteAllSamples();
		ls.lock()->openScreen(sampler.lock()->getPreviousScreenName());
		break;
	}
}
