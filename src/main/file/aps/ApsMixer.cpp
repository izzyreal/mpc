#include <file/aps/ApsMixer.hpp>

#include <mpc/MpcStereoMixerChannel.hpp>
#include <mpc/MpcIndivFxMixerChannel.hpp>

using namespace mpc::file::aps;
using namespace ctoot::mpc;

ApsMixer::ApsMixer(const std::vector<char>& loadBytes)
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

ApsMixer::ApsMixer(std::vector<std::shared_ptr<MpcStereoMixerChannel>>& smcs, std::vector<std::shared_ptr<MpcIndivFxMixerChannel>>& ifmcs)
{
	for (int i = 0; i < 64; i++) {
		auto mixerChannel = smcs[i];
		auto indivFxMixerChannel = ifmcs[i];
		saveBytes[(i * 6) + 0] = (int8_t)(indivFxMixerChannel->getFxPath());
		saveBytes[(i * 6) + 1] = (int8_t)(mixerChannel->getLevel());
		saveBytes[(i * 6) + 2] = (int8_t)((mixerChannel->getPanning()));
		saveBytes[(i * 6) + 3] = (int8_t)(indivFxMixerChannel->getVolumeIndividualOut());
		saveBytes[(i * 6) + 4] = (int8_t)(indivFxMixerChannel->getOutput());
		saveBytes[(i * 6) + 5] = (int8_t)(indivFxMixerChannel->getFxSendLevel());
	}
}

ctoot::mpc::MpcStereoMixerChannel ApsMixer::getStereoMixerChannel(int noteIndex)
{
	ctoot::mpc::MpcStereoMixerChannel result;
	result.setLevel(getLevel(noteIndex));
	result.setPanning(getPanning(noteIndex));
	return result;
}

ctoot::mpc::MpcIndivFxMixerChannel ApsMixer::getIndivFxMixerChannel(int noteIndex)
{
	ctoot::mpc::MpcIndivFxMixerChannel result;
	result.setVolumeIndividualOut(getIndividualLevel(noteIndex));
	result.setOutput(getIndividualOutput(noteIndex));
	result.setFxSendLevel(getSendLevel(noteIndex));
	result.setFxPath(getFxPath(noteIndex));
	return result;
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

std::vector<char> ApsMixer::getBytes()
{
    return saveBytes;
}
