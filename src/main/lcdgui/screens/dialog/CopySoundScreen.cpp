#include "CopySoundScreen.hpp"

#include <lcdgui/screens/window/NameScreen.hpp>

using namespace mpc::lcdgui::screens::dialog;
using namespace mpc::lcdgui::screens::window;

CopySoundScreen::CopySoundScreen(mpc::Mpc& mpc, const int layerIndex) 
	: ScreenComponent(mpc, "copy-sound", layerIndex)
{
}

void CopySoundScreen::open()
{
	auto previousScreenName = ls->getPreviousScreenName();

	if (previousScreenName != "name" && sampler->getSound())
	{
		newName = sampler->getSound()->getName();
		newName = sampler->addOrIncreaseNumber(newName);
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
		auto sound = sampler->getSound();
		auto newSound = sampler->copySound(sound);
		newSound.lock()->setName(newName);
		sampler->setSoundIndex(sampler->getSoundCount() - 1);
		openScreen("sound");
		break;
	}
	}
}

void CopySoundScreen::turnWheel(int i)
{
	init();
	
	if (param == "snd")
	{
		sampler->nudgeSoundIndex(i > 0);
		auto newSampleName = sampler->getSoundName(sampler->getSoundIndex());
		newSampleName = sampler->addOrIncreaseNumber(newSampleName);
		setNewName(newSampleName);
		displaySnd();
	}

    openNameScreen();
}

void CopySoundScreen::openNameScreen()
{
    init();

    const auto enterAction = [this](std::string& nameScreenName) {
        if (mpc.getSampler()->isSoundNameOccupied(nameScreenName))
        {
            return;
        }

        newName = nameScreenName;
        openScreen(name);
    };

    const auto nameScreen = mpc.screens->get<NameScreen>("name");
    nameScreen->initialize(newName, 16, enterAction, name);
    openScreen("name");
}

void CopySoundScreen::displayNewName()
{
	findField("newname")->setText(newName);
}

void CopySoundScreen::displaySnd()
{
	if (!sampler->getSound())
	{
		return;
	}

	findField("snd")->setText(sampler->getSound()->getName());
}

void CopySoundScreen::setNewName(std::string s)
{
	newName = s;
	displayNewName();
}
