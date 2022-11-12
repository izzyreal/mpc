#include "DeleteAllSoundScreen.hpp"

using namespace mpc::lcdgui::screens::dialog;

DeleteAllSoundScreen::DeleteAllSoundScreen(mpc::Mpc& mpc, const int layerIndex)
	: ScreenComponent(mpc, "delete-all-sound", layerIndex)
{
}

void DeleteAllSoundScreen::function(int i)
{
	init();
	
	switch (i)
	{
	case int(3) :
		openScreen("delete-sound");
		break;
	case 4:
		sampler->deleteAllSamples();
		openScreen(sampler->getPreviousScreenName());
		break;
	}
}
