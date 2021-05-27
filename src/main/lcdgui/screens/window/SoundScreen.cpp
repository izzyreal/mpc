#include "SoundScreen.hpp"

#include <lcdgui/screens/window/NameScreen.hpp>

using namespace mpc::lcdgui::screens::window;
using namespace moduru::lang;
using namespace std;

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

void SoundScreen::turnWheel(int i)
{
	init();
	
	if (param.compare("soundname") == 0)
	{
		auto nameScreen = mpc.screens->get<NameScreen>("name");
		nameScreen->setName(findField("soundname").lock()->getText());
        
        auto renamer = [&](string& newName) {
            if (sampler.lock()->isSoundNameOccupied(newName))
                return;

            sampler.lock()->getSound().lock()->setName(newName);
        };

        nameScreen->setRenamerAndScreenToReturnTo(renamer, "sound");
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
	auto sound = sampler.lock()->getSound().lock();

	if (!sound)
	{
		findField("soundname").lock()->setText("");
		return;
	}

	findField("soundname").lock()->setText(sound->getName());
}

void SoundScreen::displayType()
{
	auto sound = sampler.lock()->getSound().lock();

	if (!sound)
	{
		findLabel("type").lock()->setText("");
		return;
	}

	findLabel("type").lock()->setText("Type:" + string(sound->isMono() ? "MONO" : "STEREO"));
}

void SoundScreen::displayRate()
{
	auto sound = sampler.lock()->getSound().lock();

	if (!sound)
	{
		findLabel("rate").lock()->setText("");
		return;
	}

	findLabel("rate").lock()->setText("Rate: " + to_string(sound->getSampleRate()) + "Hz");
}

void SoundScreen::displaySize()
{
	auto sound = sampler.lock()->getSound().lock();

	if (!sound)
	{
		findLabel("size").lock()->setText("");
		return;
	}

	findLabel("size").lock()->setText("Size:" + StrUtil::padLeft(to_string(sound->getSampleData()->size() / 500), " ", 4) + "kbytes");
}
