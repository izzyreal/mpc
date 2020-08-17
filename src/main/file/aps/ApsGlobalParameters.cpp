#include "ApsGlobalParameters.hpp"

#include <Mpc.hpp>
#include <file/aps/ApsParser.hpp>

#include <lcdgui/Screens.hpp>
#include <lcdgui/screens/MixerSetupScreen.hpp>
#include <lcdgui/screens/DrumScreen.hpp>
#include <lcdgui/screens/PgmAssignScreen.hpp>

#include <file/BitUtil.hpp>

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens; 
using namespace moduru::file;
using namespace mpc::file::aps;
using namespace std;

ApsGlobalParameters::ApsGlobalParameters(mpc::Mpc& mpc, vector<char> loadBytes)
	: mpc(mpc)
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

ApsGlobalParameters::ApsGlobalParameters(mpc::Mpc& mpc)
	: mpc(mpc)
{
	saveBytes = vector<char>(ApsParser::PARAMETERS_LENGTH);
	
	for (int i = 0; i < saveBytes.size(); i++)
		saveBytes[i] = TEMPLATE[i];
	
	auto drumScreen = dynamic_pointer_cast<DrumScreen>(mpc.screens->getScreenComponent("drum"));

	auto const padToInternalSoundVal = drumScreen->isPadToIntSound();

	auto pgmAssignScreen = dynamic_pointer_cast<PgmAssignScreen>(mpc.screens->getScreenComponent("program-assign"));
	auto const padAssignMasterVal = pgmAssignScreen->padAssign;

	auto mixerSetupScreen = dynamic_pointer_cast<MixerSetupScreen>(mpc.screens->getScreenComponent("mixer-setup"));
	auto const stereoMixSourceDrumVal = mixerSetupScreen->isStereoMixSourceDrum();
	auto const indivFxSourceDrumVal = mixerSetupScreen->isIndivFxSourceDrum();
	auto const copyPgmMixToDrumVal = mixerSetupScreen->isCopyPgmMixToDrumEnabled();
	auto const recordMixChangesVal = mixerSetupScreen->isRecordMixChangesEnabled();
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

vector<char> ApsGlobalParameters::TEMPLATE = vector<char>{ 127, (char) (254 & 0xff), 124, (char) (238 & 0xff), 0, 0, 0, 64 };

int ApsGlobalParameters::readFxDrum(char b)
{
	for (int i = 2; i < 8; i++)
		b &= ~(1 << i);

	return b;
}

int ApsGlobalParameters::getFxDrum()
{
    return fxDrum;
}

bool ApsGlobalParameters::isPadToIntSoundEnabled()
{
    return padToInternalSound;
}

bool ApsGlobalParameters::isPadAssignMaster()
{
    return padAssignMaster;
}

bool ApsGlobalParameters::isStereoMixSourceDrum()
{
    return stereoMixSourceDrum;
}

bool ApsGlobalParameters::isIndivFxSourceDrum()
{
    return indivFxSourceDrum;
}

bool ApsGlobalParameters::isCopyPgmMixToDrumEnabled()
{
    return copyPgmMixToDrum;
}

bool ApsGlobalParameters::isRecordMixChangesEnabled()
{
    return recordMixChanges;
}

int ApsGlobalParameters::getMasterLevel()
{
    return masterLevel;
}

vector<char> ApsGlobalParameters::getBytes()
{
    return saveBytes;
}
