#include <controls/sampler/dialog/MonoToStereoControls.hpp>

#include <ui/sampler/SoundGui.hpp>
#include <sampler/Sampler.hpp>
#include <sampler/Sound.hpp>

using namespace mpc::controls::sampler::dialog;
using namespace std;

MonoToStereoControls::MonoToStereoControls(mpc::Mpc* mpc)
	: AbstractSamplerControls(mpc)
{
}

void MonoToStereoControls::turnWheel(int i)
{
	init();
	auto lSampler = sampler.lock();
	if (param.compare("lsource") == 0 && i < 0) {
		lSampler->setSoundGuiPrevSound();
	}
	else if (param.compare("lsource") == 0 && i > 0) {
		lSampler->setSoundGuiNextSound();
	}
	else if (param.compare("rsource") == 0) {
		soundGui->setRSource(lSampler->getNextSoundIndex(soundGui->getRSource(), i > 0), lSampler->getSoundCount());
	}
}

void MonoToStereoControls::function(int j)
{
	init();
	auto lSampler = sampler.lock();
	auto lLs = ls.lock();
	switch (j) {
	case 3:
		lLs->openScreen("sound");
		break;
	case 4:
		if (lSampler->getSound(soundGui->getSoundIndex()).lock()->isMono() && lSampler->getSound(soundGui->getRSource()).lock()->isMono()) {
			auto left = lSampler->getSound(soundGui->getSoundIndex()).lock();
			auto right = lSampler->getSound(soundGui->getRSource()).lock();
			vector<float> newSampleDataRight;
			if (right->getSampleRate() > left->getSampleRate()) {
				newSampleDataRight = vector<float>(left->getSampleData()->size());
				for (int i = 0; i < newSampleDataRight.size(); i++) {
					newSampleDataRight[i] = (*right->getSampleData())[i];
				}
			}
			else {
				newSampleDataRight = *right->getSampleData();

			}
			auto newSound = lSampler->addSound(left->getSampleRate()).lock();
			newSound->setName(soundGui->getNewStName());
			newSound->setMono(false);
			lSampler->mergeToStereo(left->getSampleData(), &newSampleDataRight, newSound->getSampleData());
			lLs->openScreen("sound");
		}
		else {
			return;
		}
	}
}
