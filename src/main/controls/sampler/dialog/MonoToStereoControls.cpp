#include <controls/sampler/dialog/MonoToStereoControls.hpp>

#include <ui/sampler/SoundGui.hpp>
#include <sampler/Sampler.hpp>
#include <sampler/Sound.hpp>

using namespace mpc::controls::sampler::dialog;
using namespace std;

MonoToStereoControls::MonoToStereoControls()
	: AbstractSamplerControls()
{
}

void MonoToStereoControls::turnWheel(int i)
{
	init();
	
	if (param.compare("lsource") == 0 && i < 0) {
		sampler.lock()->setSoundGuiPrevSound();
	}
	else if (param.compare("lsource") == 0 && i > 0) {
		sampler.lock()->setSoundGuiNextSound();
	}
	else if (param.compare("rsource") == 0) {
		soundGui->setRSource(sampler.lock()->getNextSoundIndex(soundGui->getRSource(), i > 0), sampler.lock()->getSoundCount());
	}
}

void MonoToStereoControls::function(int j)
{
	init();
	
	auto lLs = ls.lock();
	switch (j) {
	case 3:
		lLs->openScreen("sound");
		break;
	case 4:
		if (sampler.lock()->getSound(soundGui->getSoundIndex()).lock()->isMono() && sampler.lock()->getSound(soundGui->getRSource()).lock()->isMono()) {
			auto left = dynamic_pointer_cast<mpc::sampler::Sound>(sampler.lock()->getSound(soundGui->getSoundIndex()).lock());
			auto right = dynamic_pointer_cast<mpc::sampler::Sound>(sampler.lock()->getSound(soundGui->getRSource()).lock());
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
			auto newSound = sampler.lock()->addSound(left->getSampleRate()).lock();
			newSound->setName(soundGui->getNewStName());
			newSound->setMono(false);
			sampler.lock()->mergeToStereo(left->getSampleData(), &newSampleDataRight, newSound->getSampleData());
			lLs->openScreen("sound");
		}
		else {
			return;
		}
	}
}
