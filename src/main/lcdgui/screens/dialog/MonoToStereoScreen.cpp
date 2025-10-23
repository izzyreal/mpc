#include "MonoToStereoScreen.hpp"

#include <lcdgui/screens/window/NameScreen.hpp>

#include <memory>

using namespace mpc::lcdgui::screens::dialog;
using namespace mpc::lcdgui::screens::dialog2;
using namespace mpc::lcdgui::screens::window;

MonoToStereoScreen::MonoToStereoScreen(mpc::Mpc& mpc, const int layerIndex)
	: ScreenComponent(mpc, "mono-to-stereo", layerIndex)
{
}

void MonoToStereoScreen::open()
{
	if (sampler->getSound() && ls->isPreviousScreenNot<NameScreen, PopupScreen>())
	{
		auto name = sampler->getSound()->getName();
		name = StrUtil::trim(name);
		name = StrUtil::padRight(name, "_", 16);
		name = name.substr(0, 14);
		newStName = name + "-S";
	}

    if (ls->isPreviousScreenNot<NameScreen, PopupScreen>())
    {
        ls->setFocus("lsource");
    }

	setRSource(sampler->getSoundIndex());
	displayLSource();
	displayNewStName();
}

void MonoToStereoScreen::turnWheel(int i)
{
	init();
	
    const auto focusedFieldName = getFocusedFieldNameOrThrow();

	if (focusedFieldName == "lsource" && i < 0)
	{
		sampler->selectPreviousSound();
		displayLSource();

	}
	else if (focusedFieldName == "lsource" && i > 0)
	{
		sampler->selectNextSound();
		displayLSource();
	}
	else if (focusedFieldName == "rsource")
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
}

void MonoToStereoScreen::openNameScreen()
{
    init();

    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    if (focusedFieldName == "newstname")
    {
        const auto enterAction = [this](std::string& nameScreenName) {
            if (mpc.getSampler()->isSoundNameOccupied(nameScreenName))
            {
                return;
            }

            newStName = nameScreenName;
            mpc.getLayeredScreen()->openScreen<MonoToStereoScreen>();
        };

        const auto nameScreen = mpc.screens->get<NameScreen>();
        nameScreen->initialize(newStName, 16, enterAction, "mono-to-stereo");
        mpc.getLayeredScreen()->openScreen<NameScreen>();
    }
}

void MonoToStereoScreen::function(int j)
{
	init();
	
	switch (j)
	{
	case 3:
        mpc.getLayeredScreen()->openScreen<SoundScreen>();
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
				ls->showPopupAndAwaitInteraction("Name already used");
				return;
			}
		}

		auto left = sampler->getSound();

        auto newSampleDataRight = std::make_shared<std::vector<float>>();

		if (right->getSampleRate() > left->getSampleRate())
		{
			newSampleDataRight->resize(left->getSampleData()->size());

			for (int i = 0; i < newSampleDataRight->size(); i++)
				(*newSampleDataRight)[i] = (*right->getSampleData())[i];
		}
		else
		{
			(*newSampleDataRight) = *right->getSampleData();
		}

		auto newSound = sampler->addSound(left->getSampleRate());

        if (newSound == nullptr)
        {
            return;
        }

		newSound->setName(newStName);
		sampler->mergeToStereo(left->getSampleData(), newSampleDataRight, newSound->getMutableSampleData());
        newSound->setMono(false);
        mpc.getLayeredScreen()->openScreen<SoundScreen>();
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
