#include <controls/mixer/MixerSetupControls.hpp>

#include <ui/sampler/MixerSetupGui.hpp>
#include <ui/sampler/SamplerGui.hpp>

#include <lcdgui/Screens.hpp>
#include <lcdgui/screens/DrumScreen.hpp>

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::controls::mixer;
using namespace std;

MixerSetupControls::MixerSetupControls() 
	: AbstractMixerControls()
{
}

void MixerSetupControls::turnWheel(int i)
{
	init();

	if (param.compare("stereomixsource") == 0)
	{
		mixerSetupGui->setStereoMixSourceDrum(i > 0);
	}
	else if (param.compare("indivfxsource") == 0)
	{
		mixerSetupGui->setIndivFxSourceDrum(i > 0);
	}
	else if (param.compare("copypgmmixtodrum") == 0)
	{
		mixerSetupGui->setCopyPgmMixToDrumEnabled(i > 0);
	}
	else if (param.compare("recordmixchanges") == 0)
	{
		mixerSetupGui->setRecordMixChangesEnabled(i > 0);
	}
	else if (param.compare("masterlevel") == 0)
	{
		mixerSetupGui->setMasterLevel(mixerSetupGui->getMasterLevel() + i);
	}
	else if (param.compare("fxdrum") == 0)
	{
		mixerSetupGui->setFxDrum(mixerSetupGui->getFxDrum() + i);
	}
}

void MixerSetupControls::function(int i)
{
	init();
	if (i < 4)
	{
		auto drumScreen = dynamic_pointer_cast<DrumScreen>(Screens::getScreenComponent("drum"));

		drumScreen->setDrum(i);
		samplerGui->setPrevScreenName(csn);
		ls.lock()->openScreen("mixer");
	}
}

MixerSetupControls::~MixerSetupControls() {
}
