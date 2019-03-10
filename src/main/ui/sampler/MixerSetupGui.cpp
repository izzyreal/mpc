#include <ui/sampler/MixerSetupGui.hpp>

using namespace mpc::ui::sampler;
using namespace std;

MixerSetupGui::MixerSetupGui() 
{
	masterLevelNames = vector<string>{ u8"-\u00D9\u00DAdB", "-72dB", "-66dB", "-60dB", "-54dB", "-48dB", "-42dB", "-36dB", "-30dB", "-24dB", "-18dB", "-12dB", "-6dB", "0dB", "6dB", "12dB" };
}

int MixerSetupGui::getMasterLevel()
{
    return masterLevel;
}

void MixerSetupGui::setMasterLevel(int i)
{
	if (i < -13 || i > 2) return;
	masterLevel = i;
	setChanged();
	notifyObservers(string("masterlevel"));
}

int MixerSetupGui::getFxDrum()
{
    return fxDrum;
}

void MixerSetupGui::setFxDrum(int i)
{
	if (i < 0 || i > 3) return;
	fxDrum = i;
	setChanged();
	notifyObservers(string("fxdrum"));
}

bool MixerSetupGui::isStereoMixSourceDrum()
{
    return stereoMixSourceDrum;
}

void MixerSetupGui::setStereoMixSourceDrum(bool b)
{
    stereoMixSourceDrum = b;
    setChanged();
    notifyObservers(string("stereomixsource"));
}

bool MixerSetupGui::isIndivFxSourceDrum()
{
    return indivFxSourceDrum;
}

void MixerSetupGui::setIndivFxSourceDrum(bool b)
{
    indivFxSourceDrum = b;
    setChanged();
    notifyObservers(string("indivfxsource"));
}

bool MixerSetupGui::isCopyPgmMixToDrumEnabled()
{
    return copyPgmMixToDrumEnabled;
}

void MixerSetupGui::setCopyPgmMixToDrumEnabled(bool b)
{
    copyPgmMixToDrumEnabled = b;
    setChanged();
    notifyObservers(string("copypgmmixtodrum"));
}

bool MixerSetupGui::isRecordMixChangesEnabled()
{
    return recordMixChangesEnabled;
}

void MixerSetupGui::setRecordMixChangesEnabled(bool b)
{
    recordMixChangesEnabled = b;
    setChanged();
    notifyObservers(string("recordmixchanges"));
}

string MixerSetupGui::getMasterLevelString()
{
	return masterLevelNames[masterLevel + 13];
}
