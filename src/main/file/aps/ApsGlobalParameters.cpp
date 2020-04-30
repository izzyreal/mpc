#include "ApsGlobalParameters.hpp"

#include <Mpc.hpp>
#include <file/aps/ApsParser.hpp>

#include <ui/Uis.hpp>
#include <ui/sampler/MixerSetupGui.hpp>
#include <ui/sampler/SamplerGui.hpp>

#include <file/BitUtil.hpp>

using namespace moduru::file;
using namespace mpc::file::aps;
using namespace std;

ApsGlobalParameters::ApsGlobalParameters(vector<char> loadBytes)
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

ApsGlobalParameters::ApsGlobalParameters() 
{
	saveBytes = vector<char>(ApsParser::PARAMETERS_LENGTH);
	for (int i = 0; i < saveBytes.size(); i++)
	saveBytes[i] = TEMPLATE[i];
	auto uis = Mpc::instance().getUis().lock();
	auto const padToInternalSoundVal = uis->getSamplerGui()->isPadToIntSound();
	auto const padAssignMasterVal = uis->getSamplerGui()->isPadAssignMaster();
	auto const stereoMixSourceDrumVal = uis->getMixerSetupGui()->isStereoMixSourceDrum();
	auto const indivFxSourceDrumVal = uis->getMixerSetupGui()->isIndivFxSourceDrum();
	auto const copyPgmMixToDrumVal = uis->getMixerSetupGui()->isCopyPgmMixToDrumEnabled();
	auto const recordMixChangesVal = uis->getMixerSetupGui()->isRecordMixChangesEnabled();
	//auto const masterLevelVal = uis->getMixerSetupGui()->getMasterLevel();
	//auto const fxDrumVal = uis->getMixerSetupGui()->getFxDrum();
	saveBytes[0] = BitUtil::setBits(1, saveBytes[0], padToInternalSoundVal);
	saveBytes[1] = BitUtil::setBits(1, saveBytes[1], padAssignMasterVal);
	saveBytes[2] = BitUtil::setBits(1, saveBytes[2], stereoMixSourceDrumVal);
	saveBytes[2] = BitUtil::setBits(2, saveBytes[2], indivFxSourceDrumVal);
	saveBytes[3] = BitUtil::setBits(1, saveBytes[3], copyPgmMixToDrumVal);
	saveBytes[3] = BitUtil::setBits(16, saveBytes[3], recordMixChangesVal);
	/*
    Not sure why this is commented and not deleted, but leaving it in in case we want
    to further refine the proprietary file parsers.
	saveBytes[0] = BitUtil::setBit(saveBytes[0], 7, padToInternalSound);
	saveBytes[1] = BitUtil::setBit(saveBytes[1], 0, padAssignMaster);
	saveBytes[2] = BitUtil::setBit(saveBytes[2], 0, stereoMixSourceDrum);
	saveBytes[2] = BitUtil::setBit(saveBytes[2], 1, indivFxSourceDrum);
	saveBytes[3] = BitUtil::setBit(saveBytes[3], 0, copyPgmMixToDrum);
	saveBytes[3] = BitUtil::setBit(saveBytes[3], 4, padToInternalSound);
	*/
	saveBytes[4] = fxDrum;
	saveBytes[6] = masterLevel;
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
