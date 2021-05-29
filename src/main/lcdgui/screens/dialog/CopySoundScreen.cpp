#include "CopySoundScreen.hpp"

#include <lcdgui/screens/window/NameScreen.hpp>

using namespace mpc::lcdgui::screens::dialog;
using namespace mpc::lcdgui::screens::window;
using namespace std;

CopySoundScreen::CopySoundScreen(mpc::Mpc& mpc, const int layerIndex) 
	: ScreenComponent(mpc, "copy-sound", layerIndex)
{
}

void CopySoundScreen::open()
{
	auto previousScreenName = ls.lock()->getPreviousScreenName();

	if (previousScreenName.compare("name") != 0 && sampler.lock()->getSound().lock())
	{
		newName = sampler.lock()->getSound().lock()->getName();
		newName = sampler.lock()->addOrIncreaseNumber(newName);
	}
	displaySnd();
	displayNewName();
}

void CopySoundScreen::function(int i)
{
	init();

	switch (i)
	{
	case 3:
		openScreen("sound");
		break;
	case 4:
	{
		auto sound = sampler.lock()->getSound().lock();
		auto newSound = sampler.lock()->copySound(sound);
		newSound.lock()->setName(newName);
		sampler.lock()->setSoundIndex(sampler.lock()->getSoundCount() - 1);
		openScreen("sound");
		break;
	}
	}
}

void CopySoundScreen::turnWheel(int i)
{
	init();
	
	if (param.compare("snd") == 0)
	{
		sampler.lock()->setSoundIndex(sampler.lock()->getNextSoundIndex(sampler.lock()->getSoundIndex(), i > 0));
		auto newSampleName = sampler.lock()->getSoundName(sampler.lock()->getSoundIndex());
		newSampleName = sampler.lock()->addOrIncreaseNumber(newSampleName);
		setNewName(newSampleName);
		displaySnd();
	}
	else if (param.compare("newname") == 0)
	{
		const auto nameScreen = mpc.screens->get<NameScreen>("name");
        const auto copySoundScreen = this;
		nameScreen->setName(newName);
        
        auto renamer = [copySoundScreen](string& newName) {
            copySoundScreen->setNewName(newName);
        };

        nameScreen->setRenamerAndScreenToReturnTo(renamer, "copy-sound");
		openScreen("name");
	}
}

void CopySoundScreen::displayNewName()
{
	findField("newname").lock()->setText(newName);
}

void CopySoundScreen::displaySnd()
{
	if (!sampler.lock()->getSound().lock())
	{
		return;
	}

	findField("snd").lock()->setText(sampler.lock()->getSound().lock()->getName());
}

void CopySoundScreen::setNewName(string s)
{
	newName = s;
	displayNewName();
}
