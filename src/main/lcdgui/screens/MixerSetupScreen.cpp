#include "MixerSetupScreen.hpp"

#include "Mpc.hpp"
#include "StrUtil.hpp"
#include "lcdgui/LayeredScreen.hpp"
#include "lcdgui/screens/DrumScreen.hpp"

#include "engine/EngineHost.hpp"

using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui;

MixerSetupScreen::MixerSetupScreen(Mpc &mpc, const int layerIndex)
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
    const auto drumScreen = mpc.screens->get<ScreenId::DrumScreen>();
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

void MixerSetupScreen::displayFxDrum() const
{
    findField("fxdrum")->setText(std::to_string(getFxDrum() + 1));
}

void MixerSetupScreen::displayStereoMixSource() const
{
    findField("stereomixsource")
        ->setText(isStereoMixSourceDrum() ? "DRUM" : "PROGRAM");
}

void MixerSetupScreen::displayIndivFxSource() const
{
    findField("indivfxsource")
        ->setText(isIndivFxSourceDrum() ? "DRUM" : "PROGRAM");
}

void MixerSetupScreen::displayCopyPgmMixToDrum() const
{
    findField("copypgmmixtodrum")
        ->setText(isCopyPgmMixToDrumEnabled() ? "YES" : "NO");
}

void MixerSetupScreen::displayRecordMixChanges() const
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
        const auto drumScreen = mpc.screens->get<ScreenId::DrumScreen>();
        drumScreen->setDrum(i);
        openScreenById(ScreenId::MixerScreen);
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

    mpc.getEngineHost()->setMixerMasterLevel(
        masterLevelValues[masterLevel + 13]);

    displayMasterLevel();
}

int MixerSetupScreen::getFxDrum() const
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

bool MixerSetupScreen::isStereoMixSourceDrum() const
{
    return stereoMixSourceDrum;
}

void MixerSetupScreen::setStereoMixSourceDrum(bool b)
{
    stereoMixSourceDrum = b;
    displayStereoMixSource();
}

bool MixerSetupScreen::isIndivFxSourceDrum() const
{
    return indivFxSourceDrum;
}

void MixerSetupScreen::setIndivFxSourceDrum(bool b)
{
    indivFxSourceDrum = b;
    displayIndivFxSource();
}

bool MixerSetupScreen::isCopyPgmMixToDrumEnabled() const
{
    return copyPgmMixToDrumEnabled;
}

void MixerSetupScreen::setCopyPgmMixToDrumEnabled(bool b)
{
    copyPgmMixToDrumEnabled = b;
    displayCopyPgmMixToDrum();
}

bool MixerSetupScreen::isRecordMixChangesEnabled() const
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
