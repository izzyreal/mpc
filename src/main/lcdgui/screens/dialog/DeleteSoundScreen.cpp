#include "DeleteSoundScreen.hpp"

using namespace mpc::lcdgui::screens::dialog;
using namespace std;

DeleteSoundScreen::DeleteSoundScreen(mpc::Mpc& mpc, const int layerIndex)
	: ScreenComponent(mpc, "delete-sound", layerIndex)
{
}

void DeleteSoundScreen::open()
{
	displaySnd();
}

void DeleteSoundScreen::function(int i)
{
    init();
	
	switch (i)
	{
    case 2:
        openScreen("delete-all-sound");
        break;
    case 3:
        openScreen("sound");
        break;
    case 4:
        sampler->deleteSound(sampler->getSoundIndex());

		if (sampler->getSoundIndex() > sampler->getSoundCount() - 1)
			sampler->setSoundIndex(sampler->getSoundCount() - 1);
		
		if (sampler->getSoundCount() > 0)
			openScreen("sound");
		else
			openScreen(sampler->getPreviousScreenName());

		break;
    }
}

void DeleteSoundScreen::turnWheel(int i)
{
	init();
	if (param == "snd")
	{
		sampler->setSoundIndex(sampler->getSoundIndex() + i);
		displaySnd();
	}
}

void DeleteSoundScreen::displaySnd()
{
	if (!sampler->getSound().lock())
	{
		return;
	}

	findField("snd").lock()->setText(sampler->getSound().lock()->getName());
}
