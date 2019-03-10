#include <controls/sampler/dialog/DeleteAllSoundControls.hpp>

#include <ui/sampler/SoundGui.hpp>
#include <sampler/Sampler.hpp>

using namespace mpc::controls::sampler::dialog;
using namespace std;

DeleteAllSoundControls::DeleteAllSoundControls(mpc::Mpc* mpc)
	: AbstractSamplerControls(mpc)
{
}

void DeleteAllSoundControls::function(int i)
{
	init();
	auto lSampler = sampler.lock();
	auto lLs = ls.lock();
	switch (i) {
	case int(3) :
		lLs->openScreen("deletesound");
		break;
	case 4:
		lSampler->deleteAllSamples();
		lLs->openScreen(soundGui->getPreviousScreenName());
		break;
	}
}
