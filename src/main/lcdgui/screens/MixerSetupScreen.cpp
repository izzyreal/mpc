#include "MixerSetupScreen.hpp"

#include "StrUtil.hpp"
#include "lcdgui/screens/DrumScreen.hpp"

#include "audiomidi/AudioMidiServices.hpp"

using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui;

MixerSetupScreen::MixerSetupScreen(mpc::Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "mixer-setup", layerIndex)
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
    auto drumScreen = mpc.screens->get<DrumScreen>();
    ls->setFunctionKeysArrangement(drumScreen->getDrum());
}

void MixerSetupScreen::displayMasterLevel()
{

    auto level = getMasterLevelString();

    if (getMasterLevel() != -13)
    {
        level = StrUtil::padLeft(level, " ", 5);
    }

    findField("masterlevel")->setText(level);
}

void MixerSetupScreen::displayFxDrum()
{
    findField("fxdrum")->setText(std::to_string(getFxDrum() + 1));
}

void MixerSetupScreen::displayStereoMixSource()
{
    findField("stereomixsource")
        ->setText(isStereoMixSourceDrum() ? "DRUM" : "PROGRAM");
}

void MixerSetupScreen::displayIndivFxSource()
{
    findField("indivfxsource")
        ->setText(isIndivFxSourceDrum() ? "DRUM" : "PROGRAM");
}

void MixerSetupScreen::displayCopyPgmMixToDrum()
{
    findField("copypgmmixtodrum")
        ->setText(isCopyPgmMixToDrumEnabled() ? "YES" : "NO");
}

void MixerSetupScreen::displayRecordMixChanges()
{
    findField("recordmixchanges")
        ->setText(isRecordMixChangesEnabled() ? "YES" : "NO");
}

void MixerSetupScreen::turnWheel(int i)
{

    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    if (focusedFieldName == "stereomixsource")
    {
        setStereoMixSourceDrum(i > 0);
    }
    else if (focusedFieldName == "indivfxsource")
    {
        setIndivFxSourceDrum(i > 0);
    }
    else if (focusedFieldName == "copypgmmixtodrum")
    {
        setCopyPgmMixToDrumEnabled(i > 0);
    }
    else if (focusedFieldName == "recordmixchanges")
    {
        setRecordMixChangesEnabled(i > 0);
    }
    else if (focusedFieldName == "masterlevel")
    {
        setMasterLevel(masterLevel + i);
    }
    else if (focusedFieldName == "fxdrum")
    {
        setFxDrum(fxDrum + i);
    }
}

void MixerSetupScreen::function(int i)
{

    if (i < 4)
    {
        auto drumScreen = mpc.screens->get<DrumScreen>();
        drumScreen->setDrum(i);
        mpc.getLayeredScreen()->openScreen<MixerScreen>();
    }
}

int MixerSetupScreen::getMasterLevel() const
{
    return masterLevel;
}

void MixerSetupScreen::setMasterLevel(int i)
{
    if (i < -13 || i > 2)
    {
        return;
    }
    masterLevel = i;

    mpc.getAudioMidiServices()->setMixerMasterLevel(
        masterLevelValues[masterLevel + 13]);

    displayMasterLevel();
}

int MixerSetupScreen::getFxDrum()
{
    return fxDrum;
}

void MixerSetupScreen::setFxDrum(int i)
{
    if (i < 0 || i > 3)
    {
        return;
    }
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

std::string MixerSetupScreen::getMasterLevelString()
{
    return masterLevelNames[masterLevel + 13];
}
