#include <controls/sampler/dialog/DeleteSoundControls.hpp>

#include <ui/sampler/SoundGui.hpp>
#include <sampler/Sampler.hpp>

using namespace mpc::controls::sampler::dialog;
using namespace std;

DeleteSoundControls::DeleteSoundControls() : ScreenComponent("", layerIndex)
{
}

void DeleteSoundControls::function(int i)
{
    init();
	
	switch (i)
	{
    case 2:
        ls.lock()->openScreen("deleteallsound");
        break;
    case int (3):
        ls.lock()->openScreen("sound");
        break;
    case 4:
        sampler.lock()->deleteSample(sampler.lock()->getSoundIndex());

		if (sampler.lock()->getSoundIndex() > sampler.lock()->getSoundCount() - 1)
		{
			sampler.lock()->setSoundIndex(sampler.lock()->getSoundCount() - 1);
		}
		
		if (sampler.lock()->getSoundCount() > 0)
		{
			ls.lock()->openScreen("sound");
		}
		else
		{
			ls.lock()->openScreen(sampler.lock()->getPreviousScreenName());
		}
        break;
    }
}

void DeleteSoundControls::turnWheel(int i)
{
	init();
	if (param.compare("snd") == 0)
	{
		sampler.lock()->setSoundIndex(sampler.lock()->getSoundIndex() + i);
	}
}
