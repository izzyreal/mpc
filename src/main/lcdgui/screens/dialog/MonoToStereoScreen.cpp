#include "MonoToStereoScreen.hpp"

#include <ui/sampler/SoundGui.hpp>
#include <sampler/Sampler.hpp>
#include <sampler/Sound.hpp>

using namespace mpc::lcdgui::screens::dialog;
using namespace std;

MonoToStereoScreen::MonoToStereoScreen(const int layerIndex)
	: ScreenComponent("monotostereo", layerIndex)
{
}

void MonoToStereoScreen::open()
{
	displayLSource();
	displayRSource();
	displayNewStName();
}

void MonoToStereoScreen::turnWheel(int i)
{
	init();
	
	if (param.compare("lsource") == 0 && i < 0)
	{
		sampler.lock()->setSoundGuiPrevSound();
		displayLSource();
	}
	else if (param.compare("lsource") == 0 && i > 0)
	{
		sampler.lock()->setSoundGuiNextSound();
		displayLSource();
	}
	else if (param.compare("rsource") == 0)
	{
		auto soundGui = mpc.getUis().lock()->getSoundGui();
		soundGui->setRSource(sampler.lock()->getNextSoundIndex(soundGui->getRSource(), i > 0), sampler.lock()->getSoundCount());
		displayRSource();
	}
}

void MonoToStereoScreen::function(int j)
{
	init();
	
	switch (j)
	{
	case 3:
		ls.lock()->openScreen("sound");
		break;
	case 4:
	{
		auto soundGui = mpc.getUis().lock()->getSoundGui();

		if (!sampler.lock()->getSound().lock()->isMono() || !sampler.lock()->getSound(soundGui->getRSource()).lock()->isMono())
		{
			return;
		}

		auto left = sampler.lock()->getSound().lock();
		auto right = dynamic_pointer_cast<mpc::sampler::Sound>(sampler.lock()->getSound(soundGui->getRSource()).lock());

		vector<float> newSampleDataRight;

		if (right->getSampleRate() > left->getSampleRate())
		{
			newSampleDataRight = vector<float>(left->getSampleData()->size());

			for (int i = 0; i < newSampleDataRight.size(); i++)
			{
				newSampleDataRight[i] = (*right->getSampleData())[i];
			}
		}
		else
		{
			newSampleDataRight = *right->getSampleData();

		}

		auto newSound = sampler.lock()->addSound(left->getSampleRate()).lock();
		newSound->setName(soundGui->getNewStName());
		newSound->setMono(false);
		sampler.lock()->mergeToStereo(left->getSampleData(), &newSampleDataRight, newSound->getSampleData());
		ls.lock()->openScreen("sound");
	}
	}
}

void MonoToStereoScreen::displayLSource()
{
	findField("lsource").lock()->setText(sampler.lock()->getSound().lock()->getName());

	auto soundGui = mpc.getUis().lock()->getSoundGui();

	if (sampler.lock()->getSound().lock()->isMono() && sampler.lock()->getSound().lock()->isMono())
	{
		ls.lock()->setFunctionKeysArrangement(0);
	}
	else
	{
		ls.lock()->setFunctionKeysArrangement(1);
	}
}

void MonoToStereoScreen::displayRSource()
{
	auto soundGui = mpc.getUis().lock()->getSoundGui();
	findField("rsource").lock()->setText(sampler.lock()->getSoundName(soundGui->getRSource()));

	if (sampler.lock()->getSound().lock()->isMono() && sampler.lock()->getSound(soundGui->getRSource()).lock()->isMono())
	{
		ls.lock()->setFunctionKeysArrangement(0);
	}
	else
	{
		ls.lock()->setFunctionKeysArrangement(1);
	}
}

void MonoToStereoScreen::displayNewStName()
{
	auto soundGui = mpc.getUis().lock()->getSoundGui();
	findField("newstname").lock()->setText(soundGui->getNewStName());
}
