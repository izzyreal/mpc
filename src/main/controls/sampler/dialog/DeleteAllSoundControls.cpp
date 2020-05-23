#include <controls/sampler/dialog/DeleteAllSoundControls.hpp>

#include <ui/sampler/SoundGui.hpp>
#include <sampler/Sampler.hpp>

using namespace mpc::controls::sampler::dialog;
using namespace std;

DeleteAllSoundControls::DeleteAllSoundControls()
	: AbstractSamplerControls()
{
}

void DeleteAllSoundControls::function(int i)
{
	init();
	
	switch (i)
	{
	case int(3) :
		ls.lock()->openScreen("deletesound");
		break;
	case 4:
		sampler.lock()->deleteAllSamples();
		ls.lock()->openScreen(sampler.lock()->getPreviousScreenName());
		break;
	}
}
