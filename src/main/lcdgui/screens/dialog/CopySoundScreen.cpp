#include "CopySoundScreen.hpp"

#include <ui/sampler/SoundGui.hpp>
#include <ui/NameGui.hpp>

using namespace mpc::lcdgui::screens::dialog;
using namespace std;

CopySoundScreen::CopySoundScreen(const int layerIndex) 
	: ScreenComponent("copy-sound", layerIndex)
{
}

void CopySoundScreen::open()
{
	auto previousScreenName = ls.lock()->getPreviousScreenName();

	if (previousScreenName.compare("name") != 0)
	{
		newName = sampler.lock()->getSound().lock()->getName();
		//newSampleName = newSampleName->replaceAll("\\s+$", "");
		newName = sampler.lock()->addOrIncreaseNumber(newName);
	}
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
		newSound.lock()->setName(newName);
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
		setNewName(newSampleName);
		displaySnd();
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
	findField("newname").lock()->setText(newName);
}

void CopySoundScreen::displaySnd()
{
	findField("snd").lock()->setText(sampler.lock()->getSound().lock()->getName());
}

void CopySoundScreen::setNewName(string s)
{
	newName = s;
	displayNewName();
}
