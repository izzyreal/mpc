#include "MixerSetupScreen.hpp"

#include <lcdgui/screens/DrumScreen.hpp>

using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui;
using namespace moduru::lang;
using namespace std;

MixerSetupScreen::MixerSetupScreen(const int layerIndex)
	: ScreenComponent("mixer-setup", layerIndex)
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

	auto level = getMasterLevelString();

	if (getMasterLevel() != -13)
	{
		level = StrUtil::padLeft(level, " ", 5);
	}

	findField("masterlevel").lock()->setText(level);
}

void MixerSetupScreen::displayFxDrum() {
	findField("fxdrum").lock()->setText(to_string(getFxDrum() + 1));
}

void MixerSetupScreen::displayStereoMixSource() {
	findField("stereomixsource").lock()->setText(isStereoMixSourceDrum() ? "DRUM" : "PROGRAM");
}

void MixerSetupScreen::displayIndivFxSource() {
	findField("indivfxsource").lock()->setText(isIndivFxSourceDrum() ? "DRUM" : "PROGRAM");
}

void MixerSetupScreen::displayCopyPgmMixToDrum() {
	findField("copypgmmixtodrum").lock()->setText(isCopyPgmMixToDrumEnabled() ? "YES" : "NO");
}

void MixerSetupScreen::displayRecordMixChanges() {
	findField("recordmixchanges").lock()->setText(isRecordMixChangesEnabled() ? "YES" : "NO");
}

void MixerSetupScreen::turnWheel(int i)
{
	init();

	if (param.compare("stereomixsource") == 0)
	{
		setStereoMixSourceDrum(i > 0);
	}
	else if (param.compare("indivfxsource") == 0)
	{
		setIndivFxSourceDrum(i > 0);
	}
	else if (param.compare("copypgmmixtodrum") == 0)
	{
		setCopyPgmMixToDrumEnabled(i > 0);
	}
	else if (param.compare("recordmixchanges") == 0)
	{
		setRecordMixChangesEnabled(i > 0);
	}
	else if (param.compare("masterlevel") == 0)
	{
		setMasterLevel(masterLevel + i);
	}
	else if (param.compare("fxdrum") == 0)
	{
		setFxDrum(fxDrum + i);
	}
}

void MixerSetupScreen::function(int i)
{
	init();

	if (i < 4)
	{
		auto drumScreen = dynamic_pointer_cast<DrumScreen>(Screens::getScreenComponent("drum"));
		drumScreen->drum = i;
		mpc.setPreviousSamplerScreenName(currentScreenName);
		ls.lock()->openScreen("mixer");
	}
}

int MixerSetupScreen::getMasterLevel()
{
	return masterLevel;
}

void MixerSetupScreen::setMasterLevel(int i)
{
	if (i < -13 || i > 2) return;
	masterLevel = i;
	displayMasterLevel();
}

int MixerSetupScreen::getFxDrum()
{
	return fxDrum;
}

void MixerSetupScreen::setFxDrum(int i)
{
	if (i < 0 || i > 3) return;
	fxDrum = i;
	displayFxDrum();
}

bool MixerSetupScreen::isStereoMixSourceDrum()
{
	return stereoMixSourceDrum;
}

void MixerSetupScreen::setStereoMixSourceDrum(bool b)
{
	stereoMixSourceDrum = b;
	displayStereoMixSource();
}

bool MixerSetupScreen::isIndivFxSourceDrum()
{
	return indivFxSourceDrum;
}

void MixerSetupScreen::setIndivFxSourceDrum(bool b)
{
	indivFxSourceDrum = b;
	displayIndivFxSource();
}

bool MixerSetupScreen::isCopyPgmMixToDrumEnabled()
{
	return copyPgmMixToDrumEnabled;
}

void MixerSetupScreen::setCopyPgmMixToDrumEnabled(bool b)
{
	copyPgmMixToDrumEnabled = b;
	displayCopyPgmMixToDrum();
}

bool MixerSetupScreen::isRecordMixChangesEnabled()
{
	return recordMixChangesEnabled;
}

void MixerSetupScreen::setRecordMixChangesEnabled(bool b)
{
	recordMixChangesEnabled = b;
	displayRecordMixChanges();
}

string MixerSetupScreen::getMasterLevelString()
{
	return masterLevelNames[masterLevel + 13];
}
