#include "DeleteAllSoundScreen.hpp"

using namespace mpc::lcdgui::screens::dialog;

DeleteAllSoundScreen::DeleteAllSoundScreen(mpc::Mpc& mpc, const int layerIndex)
	: ScreenComponent(mpc, "delete-all-sound", layerIndex)
{
}

void DeleteAllSoundScreen::function(int i)
{
	
	switch (i)
	{
	case int(3) :
        mpc.getLayeredScreen()->openScreen<DeleteSoundScreen>();
		break;
	case 4:
		sampler->deleteAllSamples();
		mpc.getLayeredScreen()->openScreen(sampler->getPreviousScreenName());
		break;
	}
}
