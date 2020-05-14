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
	
	auto lLs = ls.lock();
	if (param.compare("stereosource") == 0) {
		soundGui->setSoundIndex(sampler.lock()->getNextSoundIndex(soundGui->getSoundIndex(), i > 0), sampler.lock()->getSoundCount());
	}
	else if (param.compare("newlname") == 0) {
		nameGui->setName(lLs->lookupField("newlname").lock()->getText());
		nameGui->setParameterName("newlname");
		lLs->openScreen("name");
	}
	else if (param.compare("newrname") == 0) {
		nameGui->setName(lLs->lookupField("newrname").lock()->getText());
		nameGui->setParameterName("newrname");
		lLs->openScreen("name");
	}
}

void StereoToMonoControls::function(int i)
{
	init();
	
	auto lLs = ls.lock();
	weak_ptr<mpc::sampler::Sound> sound;
	weak_ptr<mpc::sampler::Sound> left;
	weak_ptr<mpc::sampler::Sound> right;
	switch (i) {
	case 3:
		lLs->openScreen("sound");
		break;
	case 4:
		sound = dynamic_pointer_cast<mpc::sampler::Sound>(sampler.lock()->getSound(soundGui->getSoundIndex()).lock());
		auto lSound = sound.lock();
		if (lSound->isMono()) return;

		left = sampler.lock()->addSound(lSound->getSampleRate());
		right = sampler.lock()->addSound(lSound->getSampleRate());
		auto lLeft = left.lock();
		auto lRight = right.lock();
		lLeft->setName(soundGui->getNewLName());
		lRight->setName(soundGui->getNewRName());
		lLeft->setMono(true);
		lRight->setMono(true);
		auto leftData = lLeft->getSampleData();
		auto rightData = lRight->getSampleData();
		for (int i = 0; i <= lSound->getLastFrameIndex(); i++) {
			leftData->push_back((*lSound->getSampleData())[i]);
			rightData->push_back((*lSound->getSampleData())[i + lSound->getLastFrameIndex() + 1]);
		}
		lLeft->setEnd(lLeft->getSampleData()->size());
		lRight->setEnd(lRight->getSampleData()->size());
		lLs->openScreen("sound");
		break;
	}
}
