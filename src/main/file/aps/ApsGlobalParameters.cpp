#include "ApsGlobalParameters.hpp"

#include "Mpc.hpp"
#include "file/aps/ApsParser.hpp"

#include "lcdgui/screens/MixerSetupScreen.hpp"
#include "lcdgui/screens/DrumScreen.hpp"
#include "lcdgui/screens/PgmAssignScreen.hpp"

#include "file/BitUtil.hpp"
#include "sampler/Sampler.hpp"

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::file::aps;

ApsGlobalParameters::ApsGlobalParameters(const std::vector<char> &loadBytes)
{
    padToInternalSound = BitUtil::getBits(loadBytes[0])[7] == '1';
    padAssignMaster = BitUtil::getBits(loadBytes[1])[7] == '1';
    stereoMixSourceDrum = BitUtil::getBits(loadBytes[2])[7] == '1';
    indivFxSourceDrum = BitUtil::getBits(loadBytes[2])[6] == '1';
    copyPgmMixToDrum = BitUtil::getBits(loadBytes[3])[7] == '1';
    recordMixChanges = BitUtil::getBits(loadBytes[3])[3] == '1';
    fxDrum = readFxDrum(loadBytes[4]);
    masterLevel = loadBytes[6];
}

ApsGlobalParameters::ApsGlobalParameters(mpc::Mpc &mpc)
{
    saveBytes = std::vector<char>(ApsParser::PARAMETERS_LENGTH);

    for (int i = 0; i < saveBytes.size(); i++)
    {
        if (mpc.getSampler()->getSoundCount() == 0)
        {
            saveBytes[i] = TEMPLATE_NO_SOUNDS[i];
        }
        else
        {
            saveBytes[i] = TEMPLATE_SOUNDS[i];
        }
    }

    auto drumScreen = mpc.screens->get<ScreenId::DrumScreen>();

    auto const padToInternalSoundVal = drumScreen->isPadToIntSound();

    auto pgmAssignScreen = mpc.screens->get<ScreenId::PgmAssignScreen>();
    auto const padAssignMasterVal = pgmAssignScreen->padAssign;

    auto mixerSetupScreen = mpc.screens->get<ScreenId::MixerSetupScreen>();
    auto const stereoMixSourceDrumVal =
        mixerSetupScreen->isStereoMixSourceDrum();
    auto const indivFxSourceDrumVal = mixerSetupScreen->isIndivFxSourceDrum();
    auto const copyPgmMixToDrumVal =
        mixerSetupScreen->isCopyPgmMixToDrumEnabled();
    auto const recordMixChangesVal =
        mixerSetupScreen->isRecordMixChangesEnabled();
    auto const masterLevelVal = mixerSetupScreen->getMasterLevel();
    auto const fxDrumVal = mixerSetupScreen->getFxDrum();

    saveBytes[0] = BitUtil::setBits(1, saveBytes[0], padToInternalSoundVal);
    saveBytes[1] = BitUtil::setBits(1, saveBytes[1], padAssignMasterVal);
    saveBytes[2] = BitUtil::setBits(1, saveBytes[2], stereoMixSourceDrumVal);
    saveBytes[2] = BitUtil::setBits(2, saveBytes[2], indivFxSourceDrumVal);
    saveBytes[3] = BitUtil::setBits(1, saveBytes[3], copyPgmMixToDrumVal);
    saveBytes[3] = BitUtil::setBits(16, saveBytes[3], recordMixChangesVal);
    saveBytes[4] = fxDrumVal;
    saveBytes[6] = masterLevelVal;
}

std::vector<char> ApsGlobalParameters::TEMPLATE_NO_SOUNDS = {
    127, (char)(254 & 0xff), 124, (char)(238 & 0xff), 0, 0, 0, 64};
std::vector<char> ApsGlobalParameters::TEMPLATE_SOUNDS = {
    '\x03', '\x38', '\x00', '\xa1', 0, 0, 0, 64};

int ApsGlobalParameters::readFxDrum(char b) const
{
    for (int i = 2; i < 8; i++)
    {
        b &= ~(1 << i);
    }

    return b;
}

int ApsGlobalParameters::getFxDrum() const
{
    return fxDrum;
}

bool ApsGlobalParameters::isPadToIntSoundEnabled() const
{
    return padToInternalSound;
}

bool ApsGlobalParameters::isPadAssignMaster() const
{
    return padAssignMaster;
}

bool ApsGlobalParameters::isStereoMixSourceDrum() const
{
    return stereoMixSourceDrum;
}

bool ApsGlobalParameters::isIndivFxSourceDrum() const
{
    return indivFxSourceDrum;
}

bool ApsGlobalParameters::isCopyPgmMixToDrumEnabled() const
{
    return copyPgmMixToDrum;
}

bool ApsGlobalParameters::isRecordMixChangesEnabled() const
{
    return recordMixChanges;
}

int ApsGlobalParameters::getMasterLevel() const
{
    return masterLevel;
}

std::vector<char> ApsGlobalParameters::getBytes()
{
    return saveBytes;
}
