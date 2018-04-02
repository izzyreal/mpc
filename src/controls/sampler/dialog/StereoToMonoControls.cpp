#include <controls/sampler/dialog/StereoToMonoControls.hpp>

#include <ui/NameGui.hpp>
#include <lcdgui/Field.hpp>
#include <ui/sampler/SoundGui.hpp>
#include <sampler/Sampler.hpp>
#include <sampler/Sound.hpp>

using namespace mpc::controls::sampler::dialog;
using namespace std;

StereoToMonoControls::StereoToMonoControls(mpc::Mpc* mpc)
	: AbstractSamplerControls(mpc)
{
}

void StereoToMonoControls::turnWheel(int i)
{
	init();
	auto lSampler = sampler.lock();
	auto lLs = ls.lock();
	if (param.compare("stereosource") == 0) {
		soundGui->setSoundIndex(lSampler->getNextSoundIndex(soundGui->getSoundIndex(), i > 0), lSampler->getSoundCount());
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
	auto lSampler = sampler.lock();
	auto lLs = ls.lock();
	weak_ptr<mpc::sampler::Sound> sound;
	weak_ptr<mpc::sampler::Sound> left;
	weak_ptr<mpc::sampler::Sound> right;
	switch (i) {
	case 3:
		lLs->openScreen("sound");
		break;
	case 4:
		sound = lSampler->getSound(soundGui->getSoundIndex());
		auto lSound = sound.lock();
		if (lSound->isMono()) return;

		left = lSampler->addSound(lSound->getSampleRate());
		right = lSampler->addSound(lSound->getSampleRate());
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
