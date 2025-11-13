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
    ls->setFunctionKeysArrangement(
        sequencer::drumBusTypeToDrumIndex(drumScreen->getDrum()));
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

void MixerSetupScreen::turnWheel(const int increment)
{
    if (const auto focusedFieldName = getFocusedFieldNameOrThrow();
        focusedFieldName == "stereomixsource")
    {
        setStereoMixSourceDrum(increment > 0);
    }
    else if (focusedFieldName == "indivfxsource")
    {
        setIndivFxSourceDrum(increment > 0);
    }
    else if (focusedFieldName == "copypgmmixtodrum")
    {
        setCopyPgmMixToDrumEnabled(increment > 0);
    }
    else if (focusedFieldName == "recordmixchanges")
    {
        setRecordMixChangesEnabled(increment > 0);
    }
    else if (focusedFieldName == "masterlevel")
    {
        setMasterLevel(masterLevel + increment);
    }
    else if (focusedFieldName == "fxdrum")
    {
        setFxDrum(fxDrum + increment);
    }
}

void MixerSetupScreen::function(const int i)
{
    if (i < 4)
    {
        const auto drumScreen = mpc.screens->get<ScreenId::DrumScreen>();
        drumScreen->setDrum(sequencer::drumBusIndexToDrumBusType(i));
        openScreenById(ScreenId::MixerScreen);
    }
}

int MixerSetupScreen::getMasterLevel() const
{
    return masterLevel;
}

void MixerSetupScreen::setMasterLevel(const int i)
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

void MixerSetupScreen::setFxDrum(const int i)
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

void MixerSetupScreen::setStereoMixSourceDrum(const bool b)
{
    stereoMixSourceDrum = b;
    displayStereoMixSource();
}

bool MixerSetupScreen::isIndivFxSourceDrum() const
{
    return indivFxSourceDrum;
}

void MixerSetupScreen::setIndivFxSourceDrum(const bool b)
{
    indivFxSourceDrum = b;
    displayIndivFxSource();
}

bool MixerSetupScreen::isCopyPgmMixToDrumEnabled() const
{
    return copyPgmMixToDrumEnabled;
}

void MixerSetupScreen::setCopyPgmMixToDrumEnabled(const bool b)
{
    copyPgmMixToDrumEnabled = b;
    displayCopyPgmMixToDrum();
}

bool MixerSetupScreen::isRecordMixChangesEnabled() const
{
    return recordMixChangesEnabled;
}

void MixerSetupScreen::setRecordMixChangesEnabled(const bool b)
{
    recordMixChangesEnabled = b;
    displayRecordMixChanges();
}

std::string MixerSetupScreen::getMasterLevelString()
{
    return masterLevelNames[masterLevel + 13];
}
