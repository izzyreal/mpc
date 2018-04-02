#include <controls/sampler/dialog/DeleteSoundControls.hpp>

#include <ui/sampler/SoundGui.hpp>
#include <sampler/Sampler.hpp>

using namespace mpc::controls::sampler::dialog;
using namespace std;

DeleteSoundControls::DeleteSoundControls(mpc::Mpc* mpc) : AbstractSamplerControls(mpc)
{
}

void DeleteSoundControls::function(int i)
{
    init();
	auto lSampler = sampler.lock();
	auto lLs = ls.lock();
	switch (i) {
    case 2:
        lLs->openScreen("deleteallsound");
        break;
    case int (3):
        lLs->openScreen("sound");
        break;
    case 4:
        lSampler->deleteSample(soundGui->getSoundIndex());
		if (!soundGui->getSoundIndex() < lSampler->getSoundCount()) {
			soundGui->setSoundIndex(lSampler->getSoundCount() - 1, lSampler->getSoundCount());
		}
		if (lSampler->getSoundCount() > 0) {
			lLs->openScreen("sound");
		}
		else {
			lLs->openScreen(soundGui->getPreviousScreenName());
		}
        break;
    }
}

void DeleteSoundControls::turnWheel(int i)
{
	init();
	if (param.compare("snd") == 0) {
		soundGui->setSoundIndex(soundGui->getSoundIndex() + i, sampler.lock()->getSoundCount());
	}
}
