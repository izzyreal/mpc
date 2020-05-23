#include <controls/sampler/dialog/StereoToMonoControls.hpp>

#include <ui/NameGui.hpp>
#include <lcdgui/Field.hpp>
#include <ui/sampler/SoundGui.hpp>
#include <sampler/Sampler.hpp>
#include <sampler/Sound.hpp>

using namespace mpc::controls::sampler::dialog;
using namespace std;

StereoToMonoControls::StereoToMonoControls()
	: AbstractSamplerControls()
{
}

void StereoToMonoControls::turnWheel(int i)
{
	init();
	
	if (param.compare("stereosource") == 0)
	{
		sampler.lock()->setSoundIndex(sampler.lock()->getNextSoundIndex(sampler.lock()->getSoundIndex(), i > 0));
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

void StereoToMonoControls::function(int i)
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

		left->setName(soundGui->getNewLName());
		right->setName(soundGui->getNewRName());

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
