#include <controls/sampler/dialog/DeleteSoundControls.hpp>

#include <ui/sampler/SoundGui.hpp>
#include <sampler/Sampler.hpp>

using namespace mpc::controls::sampler::dialog;
using namespace std;

DeleteSoundControls::DeleteSoundControls() : AbstractSamplerControls()
{
}

void DeleteSoundControls::function(int i)
{
    init();
	
	auto lLs = ls.lock();
	switch (i) {
    case 2:
        lLs->openScreen("deleteallsound");
        break;
    case int (3):
        lLs->openScreen("sound");
        break;
    case 4:
        sampler.lock()->deleteSample(soundGui->getSoundIndex());
		if (soundGui->getSoundIndex() > sampler.lock()->getSoundCount() - 1) {
			soundGui->setSoundIndex(sampler.lock()->getSoundCount() - 1, sampler.lock()->getSoundCount());
		}
		if (sampler.lock()->getSoundCount() > 0) {
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
