#include "StereoToMonoScreen.hpp"

#include <ui/NameGui.hpp>
#include <ui/sampler/SoundGui.hpp>

using namespace mpc::lcdgui::screens::dialog;
using namespace moduru::lang;
using namespace std;

StereoToMonoScreen::StereoToMonoScreen(const int layerIndex)
	: ScreenComponent("stereo-to-mono", layerIndex)
{
}

void StereoToMonoScreen::open()
{
	auto previousScreenName = ls.lock()->getPreviousScreenName();

	if (previousScreenName.compare("name") != 0)
	{
		string name = sampler.lock()->getSound().lock()->getName();
		name = StrUtil::trim(name);
		name = StrUtil::padRight(name, "_", 16);
		name = name.substr(0, 14);

		setNewLName(name + "-L");
		setNewRName(name + "-R");
	}

	displayNewLName();
	displayNewRName();
	displayStereoSource();
}

void StereoToMonoScreen::turnWheel(int i)
{
	init();
	
	if (param.compare("stereosource") == 0)
	{
		sampler.lock()->setSoundIndex(sampler.lock()->getNextSoundIndex(sampler.lock()->getSoundIndex(), i > 0));
		displayStereoSource();
	}
	else if (param.compare("newlname") == 0)
	{
		nameGui->setName(ls.lock()->lookupField("newlname").lock()->getText());
		nameGui->setParameterName("newlname");
		ls.lock()->openScreen("name");
	}
	else if (param.compare("newrname") == 0)
	{
		nameGui->setName(ls.lock()->lookupField("newrname").lock()->getText());
		nameGui->setParameterName("newrname");
		ls.lock()->openScreen("name");
	}
}

void StereoToMonoScreen::function(int i)
{
	init();
		
	switch (i)
	{
	case 3:
		ls.lock()->openScreen("sound");
		break;
	case 4:
	{
		auto sound = sampler.lock()->getSound().lock();

		if (sound->isMono())
		{
			return;
		}

		auto left = sampler.lock()->addSound(sound->getSampleRate()).lock();
		auto right = sampler.lock()->addSound(sound->getSampleRate()).lock();

		auto soundGui = mpc.getUis().lock()->getSoundGui();
		left->setName(newLName);
		right->setName(newRName);

		left->setMono(true);
		right->setMono(true);
		
		auto leftData = left->getSampleData();
		auto rightData = right->getSampleData();

		for (int i = 0; i <= sound->getLastFrameIndex(); i++)
		{
			leftData->push_back((*sound->getSampleData())[i]);
			rightData->push_back((*sound->getSampleData())[i + sound->getFrameCount()]);
		}
		
		left->setEnd(left->getSampleData()->size());
		right->setEnd(right->getSampleData()->size());
		ls.lock()->openScreen("sound");
		
		break;
	}
	}
}

void StereoToMonoScreen::displayStereoSource()
{
	auto sound = sampler.lock()->getSound().lock();
	findField("stereosource").lock()->setText(sound->getName());

	if (sound->isMono())
	{
		ls.lock()->setFunctionKeysArrangement(1);
	}
	else {
		ls.lock()->setFunctionKeysArrangement(0);
	}
}

void StereoToMonoScreen::displayNewLName()
{
	auto soundGui = mpc.getUis().lock()->getSoundGui();
	findField("newlname").lock()->setText(newLName);
}

void StereoToMonoScreen::displayNewRName()
{
	auto soundGui = mpc.getUis().lock()->getSoundGui();
	findField("newrname").lock()->setText(newRName);
}

void StereoToMonoScreen::setNewLName(string s)
{
	newLName = s;
	displayNewLName();
}

void StereoToMonoScreen::setNewRName(string s)
{
	newRName = s;
	displayNewRName();
}
