#include "MonoToStereoScreen.hpp"

#include <lcdgui/screens/window/NameScreen.hpp>
#include <lcdgui/screens/dialog2/PopupScreen.hpp>

using namespace mpc::lcdgui::screens::dialog;
using namespace mpc::lcdgui::screens::dialog2;
using namespace mpc::lcdgui::screens::window;
using namespace moduru::lang;

MonoToStereoScreen::MonoToStereoScreen(mpc::Mpc& mpc, const int layerIndex)
	: ScreenComponent(mpc, "mono-to-stereo", layerIndex)
{
}

void MonoToStereoScreen::open()
{
	auto prevScreen = ls->getPreviousScreenName();

	if (sampler->getSound() && prevScreen != "name" && prevScreen != "popup")
	{
		auto name = sampler->getSound()->getName();
		name = StrUtil::trim(name);
		name = StrUtil::padRight(name, "_", 16);
		name = name.substr(0, 14);
		newStName = name + "-S";
	}

	if (prevScreen != "name" && prevScreen != "popup")
		ls->setFocus("lsource");

	setRSource(sampler->getSoundIndex());
	displayLSource();
	displayNewStName();
}

void MonoToStereoScreen::turnWheel(int i)
{
	init();
	
	if (param == "lsource" && i < 0)
	{
		sampler->selectPreviousSound();
		displayLSource();

	}
	else if (param == "lsource" && i > 0)
	{
		sampler->selectNextSound();
		displayLSource();
	}
	else if (param == "rsource")
	{
        auto nextSoundIndex = rSource;
        if (i > 0 && nextSoundIndex != sampler->getSoundCount() - 1)
        {
            nextSoundIndex++;
        }
        else if (i < 0 && nextSoundIndex != 0)
        {
            nextSoundIndex--;
        }

		setRSource(nextSoundIndex);
		displayRSource();
	}
	else if (param == "newstname")
	{
		const auto nameScreen = mpc.screens->get<NameScreen>("name");
        const auto monoToStereoScreen = this;
		nameScreen->setName(newStName);
        
        auto renamer = [monoToStereoScreen](std::string& newName) {
            monoToStereoScreen->newStName = newName;
        };

        nameScreen->setRenamerAndScreenToReturnTo(renamer, "mono-to-stereo");
		openScreen("name");
	}
}

void MonoToStereoScreen::function(int j)
{
	init();
	
	switch (j)
	{
	case 3:
		openScreen("sound");
		break;
	case 4:
	{
        auto right = sampler->getSortedSounds()[rSource].first;
		if (!sampler->getSound()->isMono() || !right->isMono())
        {
            return;
        }

		for (auto& s : sampler->getSounds())
		{
			if (s->getName() == newStName)
			{
				auto popupScreen = mpc.screens->get<PopupScreen>("popup");
				popupScreen->setText("Name already used");
				popupScreen->returnToScreenAfterInteraction(name);
				openScreen("popup");
				return;
			}
		}

		auto left = sampler->getSound();

		std::vector<float> newSampleDataRight;

		if (right->getSampleRate() > left->getSampleRate())
		{
			newSampleDataRight = std::vector<float>(left->getSampleData()->size());

			for (int i = 0; i < newSampleDataRight.size(); i++)
				newSampleDataRight[i] = (*right->getSampleData())[i];
		}
		else
		{
			newSampleDataRight = *right->getSampleData();
		}

		auto newSound = sampler->addSound(left->getSampleRate());
		newSound->setName(newStName);
		sampler->mergeToStereo(left->getSampleData(), &newSampleDataRight, newSound->getSampleData());
        newSound->setMono(false);
		openScreen("sound");
	}
	}
}

void MonoToStereoScreen::displayLSource()
{
	if (!sampler->getSound())
		return;

	findField("lsource")->setText(sampler->getSound()->getName());

	if (sampler->getSound()->isMono() && sampler->getSound()->isMono())
	{
		ls->setFunctionKeysArrangement(0);
	}
	else
	{
		ls->setFunctionKeysArrangement(1);
		findChild<Background>("")->repaintUnobtrusive(findChild<FunctionKey>("fk4")->getRect());
	}
}

void MonoToStereoScreen::displayRSource()
{
    if (rSource >= sampler->getSoundCount())
    {
        findField("rsource")->setText("");
        return;
    }

    auto sound = sampler->getSortedSounds()[rSource];

	findField("rsource")->setText(sound.first->getName());

	if (sound.first->isMono() && sampler->getSound()->isMono())
	{
		ls->setFunctionKeysArrangement(0);
	}
	else
	{
		ls->setFunctionKeysArrangement(1);
		findBackground()->repaintUnobtrusive(findChild<FunctionKey>("fk4")->getRect());
	}
}

void MonoToStereoScreen::displayNewStName()
{
	findField("newstname")->setText(newStName);
}

void MonoToStereoScreen::setRSource(int i)
{
	if (i < 0 || i >= sampler->getSoundCount())
		return;

	rSource = i;
	displayRSource();
}
