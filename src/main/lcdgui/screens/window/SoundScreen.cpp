#include "SoundScreen.hpp"

#include <lcdgui/screens/window/NameScreen.hpp>

using namespace mpc::lcdgui::screens::window;
using namespace moduru::lang;

SoundScreen::SoundScreen(mpc::Mpc& mpc, const int layerIndex) 
	: ScreenComponent(mpc, "sound", layerIndex)
{
}

void SoundScreen::open()
{
	displaySoundName();
	displayType();
	displayRate();
	displaySize();
}

void SoundScreen::openNameScreen()
{
	init();
	
	if (param == "soundname")
	{
        const auto enterAction = [this](std::string& nameScreenName) {
            if (mpc.getSampler()->isSoundNameOccupied(nameScreenName))
            {
                return;
            }

            sampler->getSound()->setName(nameScreenName);
            openScreen(name);
        };

        const auto nameScreen = mpc.screens->get<NameScreen>("name");
        nameScreen->initialize(sampler->getPreviewSound()->getName(), 16, enterAction, name);
        openScreen("name");
	}
}

void SoundScreen::function(int i)
{
	ScreenComponent::function(i);

	switch (i)
	{
	case 1:
		openScreen("delete-sound");
		break;
	case 2:
		openScreen("convert-sound");
		break;
	case 4:
		openScreen("copy-sound");
		break;
	}
}

void SoundScreen::displaySoundName()
{
	auto sound = sampler->getSound();

	if (!sound)
	{
		findField("soundname")->setText("");
		return;
	}

	findField("soundname")->setText(sound->getName());
}

void SoundScreen::displayType()
{
	auto sound = sampler->getSound();

	if (!sound)
	{
		findLabel("type")->setText("");
		return;
	}

	findLabel("type")->setText("Type:" + std::string(sound->isMono() ? "MONO" : "STEREO"));
}

void SoundScreen::displayRate()
{
	auto sound = sampler->getSound();

	if (!sound)
	{
		findLabel("rate")->setText("");
		return;
	}

	findLabel("rate")->setText("Rate: " + std::to_string(sound->getSampleRate()) + "Hz");
}

void SoundScreen::displaySize()
{
	auto sound = sampler->getSound();

	if (!sound)
	{
		findLabel("size")->setText("");
		return;
	}

	findLabel("size")->setText("Size:" + StrUtil::padLeft(std::to_string(sound->getSampleData()->size() / 500), " ", 4) + "kbytes");
}
