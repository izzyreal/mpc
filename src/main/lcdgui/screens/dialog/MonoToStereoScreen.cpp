#include "MonoToStereoScreen.hpp"

#include <sampler/Sampler.hpp>
#include <sampler/Sound.hpp>

using namespace mpc::lcdgui::screens::dialog;
using namespace moduru::lang;
using namespace std;

MonoToStereoScreen::MonoToStereoScreen(const int layerIndex)
	: ScreenComponent("monotostereo", layerIndex)
{
}

void MonoToStereoScreen::open()
{
	string name = sampler.lock()->getSound().lock()->getName();
	name = StrUtil::trim(name);
	name = StrUtil::padRight(name, "_", 16);
	name = name.substr(0, 14);
	setNewStName(name + "-S");
	setNewStName(name + "-S");

	setRSource(sampler.lock()->getSoundIndex());
	displayLSource();
	displayRSource();
}

void MonoToStereoScreen::turnWheel(int i)
{
	init();
	
	if (param.compare("lsource") == 0 && i < 0)
	{
		sampler.lock()->selectPreviousSound();
		displayLSource();
	}
	else if (param.compare("lsource") == 0 && i > 0)
	{
		sampler.lock()->selectNextSound();
		displayLSource();
	}
	else if (param.compare("rsource") == 0)
	{
		setRSource(sampler.lock()->getNextSoundIndex(rSource, i > 0));
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
		if (!sampler.lock()->getSound().lock()->isMono() || !sampler.lock()->getSound(rSource).lock()->isMono())
		{
			return;
		}

		auto left = sampler.lock()->getSound().lock();
		auto right = dynamic_pointer_cast<mpc::sampler::Sound>(sampler.lock()->getSound(rSource).lock());

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
		newSound->setName(newStName);
		newSound->setMono(false);
		sampler.lock()->mergeToStereo(left->getSampleData(), &newSampleDataRight, newSound->getSampleData());
		ls.lock()->openScreen("sound");
	}
	}
}

void MonoToStereoScreen::displayLSource()
{
	findField("lsource").lock()->setText(sampler.lock()->getSound().lock()->getName());

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
	findField("rsource").lock()->setText(sampler.lock()->getSoundName(rSource));

	if (sampler.lock()->getSound().lock()->isMono() && sampler.lock()->getSound(rSource).lock()->isMono())
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
	findField("newstname").lock()->setText(newStName);
}

void MonoToStereoScreen::setRSource(int i)
{
	if (i < 0 || i >= sampler.lock()->getSoundCount())
	{
		return;
	}

	rSource = i;
	displayRSource();
}

void MonoToStereoScreen::setNewStName(string s)
{
	newStName = s;
}

