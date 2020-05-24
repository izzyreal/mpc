#include "CopySoundScreen.hpp"

#include <ui/sampler/SoundGui.hpp>
#include <ui/NameGui.hpp>

using namespace mpc::lcdgui::screens::dialog;
using namespace std;

CopySoundScreen::CopySoundScreen(const int layerIndex) 
	: ScreenComponent("copysound", layerIndex)
{
}

void CopySoundScreen::open()
{
	displaySnd();
	displayNewName();
}

void CopySoundScreen::function(int i)
{
	init();

	switch (i) {
	case int(3) :
		ls.lock()->openScreen("sound");
		break;
	case 4:
	{
		auto soundGui = mpc.getUis().lock()->getSoundGui();
		auto sound = sampler.lock()->getSound().lock();
		auto newSound = sampler.lock()->copySound(sound);
		newSound.lock()->setName(soundGui->getNewName());
		sampler.lock()->setSoundIndex(sampler.lock()->getSoundCount() - 1);
		ls.lock()->openScreen("sound");
		break;
	}
	}
}

void CopySoundScreen::turnWheel(int i)
{
	init();
	
	if (param.compare("snd") == 0)
	{
		auto soundGui = mpc.getUis().lock()->getSoundGui();
		sampler.lock()->setSoundIndex(sampler.lock()->getNextSoundIndex(sampler.lock()->getSoundIndex(), i > 0));
		auto newSampleName = sampler.lock()->getSoundName(sampler.lock()->getSoundIndex());
		//newSampleName = newSampleName->replaceAll("\\s+$", "");
		newSampleName = sampler.lock()->addOrIncreaseNumber(newSampleName);
		soundGui->setNewName(newSampleName);
		displaySnd();
		displayNewName();
	}
	else if (param.compare("newname") == 0)
	{
		auto soundGui = mpc.getUis().lock()->getNameGui();
		nameGui->setName(ls.lock()->lookupField("newname").lock()->getText());
		nameGui->setParameterName("newname");
		ls.lock()->openScreen("name");
	}
}

void CopySoundScreen::displayNewName()
{
	auto soundGui = mpc.getUis().lock()->getSoundGui();
	findField("newname").lock()->setText(soundGui->getNewName());
}

void CopySoundScreen::displaySnd()
{
	findField("snd").lock()->setText(sampler.lock()->getSound().lock()->getName());
}
