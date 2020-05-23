#include <lcdgui/screens/SndParamsScreen.hpp>

#include <Mpc.hpp>
#include <controls/Controls.hpp>
#include <ui/sampler/SoundGui.hpp>
#include <ui/sampler/window/EditSoundGui.hpp>
#include <sampler/Sampler.hpp>
#include <sampler/Sound.hpp>

using namespace mpc::lcdgui::screens;
using namespace std;

SndParamsScreen::SndParamsScreen(const int layerIndex) 
	: ScreenComponent("params", layerIndex)
{
}

void SndParamsScreen::openWindow()
{
	init();

	if (param.compare("snd") == 0)
	{
		sampler.lock()->setPreviousScreenName("params");
		ls.lock()->openScreen("sound");
	}
}

void SndParamsScreen::function(int f)
{
	init();
	
	switch (f)
	{
	case 0:
		ls.lock()->openScreen("trim");
		break;
	case 1:
		ls.lock()->openScreen("loop");
		break;
	case 2:
		ls.lock()->openScreen("zone");
		break;
	case 3:
		sampler.lock()->sort();
		break;
	case 4:
	{
		if (sampler.lock()->getSoundCount() == 0)
		{
			return;
		}

		auto newSampleName = sampler.lock()->getSound().lock()->getName();
		//newSampleName = newSampleName->replaceAll("\\s+$", "");
		newSampleName = sampler.lock()->addOrIncreaseNumber(newSampleName);
		auto editSoundGui = mpc.getUis().lock()->getEditSoundGui();
		editSoundGui->setNewName(newSampleName);
		editSoundGui->setPreviousScreenName("trim");
		ls.lock()->openScreen("editsound");
		break;
	}
	case 5:
	{
		if (Mpc::instance().getControls().lock()->isF6Pressed())
		{
			return;
		}

		Mpc::instance().getControls().lock()->setF6Pressed(true);
		auto soundGui = mpc.getUis().lock()->getSoundGui();

		auto zone = vector<int>{ soundGui->getZoneStart(soundGui->getZoneNumber()) , soundGui->getZoneEnd(soundGui->getZoneNumber()) };
		sampler.lock()->playX(soundGui->getPlayX(), &zone);
		break;
	}
	}
}

void SndParamsScreen::turnWheel(int i)
{
	init();
	auto soundGui = mpc.getUis().lock()->getSoundGui();
	auto sound = sampler.lock()->getSound().lock();

	if (param.compare("playx") == 0)
	{
		soundGui->setPlayX(soundGui->getPlayX() + i);
	}
	else if (param.compare("snd") == 0 && i > 0)
	{
		sampler.lock()->setSoundGuiNextSound();
	}
	else if (param.compare("snd") == 0 && i < 0)
	{
		sampler.lock()->setSoundGuiPrevSound();
	}
	else if (param.compare("level") == 0)
	{
		sound->setLevel(sound->getSndLevel() + i);
	}
	else if (param.compare("tune") == 0)
	{
		sound->setTune(sound->getTune() + i);
	}
	else if (param.compare("beat") == 0)
	{
		sound->setNumberOfBeats(sound->getBeatCount() + i);
	}
}
