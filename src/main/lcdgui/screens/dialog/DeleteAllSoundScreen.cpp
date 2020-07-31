#include "DeleteAllSoundScreen.hpp"

using namespace mpc::lcdgui::screens::dialog;
using namespace std;

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
		ls.lock()->openScreen("delete-sound");
		break;
	case 4:
		sampler.lock()->deleteAllSamples();
		ls.lock()->openScreen(sampler.lock()->getPreviousScreenName());
		break;
	}
}
