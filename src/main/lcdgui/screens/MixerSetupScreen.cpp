#include "MixerSetupScreen.hpp"

#include <ui/sampler/MixerSetupGui.hpp>
#include <ui/sampler/SamplerGui.hpp>

#include <lcdgui/screens/DrumScreen.hpp>
#include <lcdgui/Screens.hpp>

using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui;
using namespace moduru::lang;
using namespace std;

MixerSetupScreen::MixerSetupScreen(const int& layer)
	: ScreenComponent("mixersetup", layer)
{
}

void MixerSetupScreen::open()
{
	displayMasterLevel();
	displayFxDrum();
	displayStereoMixSource();
	displayIndivFxSource();
	displayCopyPgmMixToDrum();
	displayRecordMixChanges();
}

void MixerSetupScreen::displayMasterLevel() {

	auto mixerSetupGui = mpc.getUis().lock()->getMixerSetupGui();
	auto level = mixerSetupGui->getMasterLevelString();

	if (mixerSetupGui->getMasterLevel() != -13)
	{
		level = StrUtil::padLeft(level, " ", 5);
	}

	findField("masterlevel").lock()->setText(level);
}

void MixerSetupScreen::displayFxDrum() {
	auto mixerSetupGui = mpc.getUis().lock()->getMixerSetupGui();
	findField("fxdrum").lock()->setText(to_string(mixerSetupGui->getFxDrum() + 1));
}

void MixerSetupScreen::displayStereoMixSource() {
	auto mixerSetupGui = mpc.getUis().lock()->getMixerSetupGui();
	findField("stereomixsource").lock()->setText(mixerSetupGui->isStereoMixSourceDrum() ? "DRUM" : "PROGRAM");
}

void MixerSetupScreen::displayIndivFxSource() {
	auto mixerSetupGui = mpc.getUis().lock()->getMixerSetupGui();
	findField("indivfxsource").lock()->setText(mixerSetupGui->isIndivFxSourceDrum() ? "DRUM" : "PROGRAM");
}

void MixerSetupScreen::displayCopyPgmMixToDrum() {
	auto mixerSetupGui = mpc.getUis().lock()->getMixerSetupGui();
	findField("copypgmmixtodrum").lock()->setText(mixerSetupGui->isCopyPgmMixToDrumEnabled() ? "YES" : "NO");
}

void MixerSetupScreen::displayRecordMixChanges() {
	auto mixerSetupGui = mpc.getUis().lock()->getMixerSetupGui();
	findField("recordmixchanges").lock()->setText(mixerSetupGui->isRecordMixChangesEnabled() ? "YES" : "NO");
}

void MixerSetupScreen::turnWheel(int i)
{
	init();

	auto mixerSetupGui = mpc.getUis().lock()->getMixerSetupGui();

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

void MixerSetupScreen::function(int i)
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
