#include <controls/mixer/MixerSetupControls.hpp>

#include <ui/sampler/MixerSetupGui.hpp>
#include <ui/sampler/SamplerGui.hpp>

using namespace mpc::controls::mixer;
using namespace std;

MixerSetupControls::MixerSetupControls() 
	: AbstractMixerControls()
{
}

void MixerSetupControls::turnWheel(int i)
{
	init();
	auto yes = i > 0;
	if (param.compare("stereomixsource") == 0) {
		mixerSetupGui->setStereoMixSourceDrum(yes);
	}
	else if (param.compare("indivfxsource") == 0) {
		mixerSetupGui->setIndivFxSourceDrum(yes);
	}
	else if (param.compare("copypgmmixtodrum") == 0) {
		mixerSetupGui->setCopyPgmMixToDrumEnabled(yes);
	}
	else if (param.compare("recordmixchanges") == 0) {
		mixerSetupGui->setRecordMixChangesEnabled(yes);
	}
	else if (param.compare("masterlevel") == 0) {
		mixerSetupGui->setMasterLevel(mixerSetupGui->getMasterLevel() + i);
	}
	else if (param.compare("fxdrum") == 0) {
		mixerSetupGui->setFxDrum(mixerSetupGui->getFxDrum() + i);
	}
}

void MixerSetupControls::function(int i)
{
	init();
	if (i < 4) {
		samplerGui->setSelectedDrum(i);
		samplerGui->setPrevScreenName(csn);
		ls.lock()->openScreen("mixer");
	}
}

MixerSetupControls::~MixerSetupControls() {
}
