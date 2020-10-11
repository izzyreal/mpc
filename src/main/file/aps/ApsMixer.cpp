#include <file/aps/ApsMixer.hpp>

#include <mpc/MpcStereoMixerChannel.hpp>
#include <mpc/MpcIndivFxMixerChannel.hpp>

using namespace mpc::file::aps;
using namespace std;

ApsMixer::ApsMixer(vector<char> loadBytes) 
{
	for (int i = 0; i < 64; i++) {
		fxPaths[i] = loadBytes[(i * 6) + 0];
		levels[i] = loadBytes[(i * 6) + 1];
		pannings[i] = (loadBytes[(i * 6) + 2]);
		iLevels[i] = loadBytes[(i * 6) + 3];
		iOutputs[i] = loadBytes[(i * 6) + 4];
		sendLevels[i] = loadBytes[(i * 6) + 5];
	}
}

ApsMixer::ApsMixer(vector<weak_ptr<ctoot::mpc::MpcStereoMixerChannel>> smcs, vector<weak_ptr<ctoot::mpc::MpcIndivFxMixerChannel>> ifmcs)
{
	for (int i = 0; i < 64; i++) {
		auto mixerChannel = smcs[i].lock();
		auto indivFxMixerChannel = ifmcs[i].lock();
		saveBytes[(i * 6) + 0] = (int8_t)(indivFxMixerChannel->getFxPath());
		saveBytes[(i * 6) + 1] = (int8_t)(mixerChannel->getLevel());
		saveBytes[(i * 6) + 2] = (int8_t)((mixerChannel->getPanning()));
		saveBytes[(i * 6) + 3] = (int8_t)(indivFxMixerChannel->getVolumeIndividualOut());
		saveBytes[(i * 6) + 4] = (int8_t)(indivFxMixerChannel->getOutput());
		saveBytes[(i * 6) + 5] = (int8_t)(indivFxMixerChannel->getFxSendLevel());
	}
}

ctoot::mpc::MpcStereoMixerChannel* ApsMixer::getStereoMixerChannel(int noteIndex)
{
	auto params = new ctoot::mpc::MpcStereoMixerChannel();
	params->setLevel(getLevel(noteIndex));
	params->setPanning(getPanning(noteIndex));
	return params;
}

ctoot::mpc::MpcIndivFxMixerChannel* ApsMixer::getIndivFxMixerChannel(int noteIndex)
{
	auto params = new ctoot::mpc::MpcIndivFxMixerChannel();
	params->setVolumeIndividualOut(getIndividualLevel(noteIndex));
	params->setOutput(getIndividualOutput(noteIndex));
	params->setFxSendLevel(getSendLevel(noteIndex));
	params->setFxPath(getFxPath(noteIndex));
	return params;
}

int ApsMixer::getFxPath(int noteIndex)
{
    return fxPaths[noteIndex];
}

int ApsMixer::getLevel(int noteIndex)
{
	return levels[noteIndex];
}

int ApsMixer::getPanning(int noteIndex)
{
    return pannings[noteIndex];
}

int ApsMixer::getIndividualLevel(int noteIndex)
{
    return iLevels[noteIndex];
}

int ApsMixer::getIndividualOutput(int noteIndex)
{
    return iOutputs[noteIndex];
}

int ApsMixer::getSendLevel(int noteIndex)
{
    return sendLevels[noteIndex];
}

vector<char> ApsMixer::getBytes()
{
    return saveBytes;
}
