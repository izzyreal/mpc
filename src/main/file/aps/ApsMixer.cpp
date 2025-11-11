#include "file/aps/ApsMixer.hpp"

#include "engine/StereoMixer.hpp"
#include "engine/IndivFxMixer.hpp"

using namespace mpc::file::aps;
using namespace mpc::engine;

ApsMixer::ApsMixer(const std::vector<char> &loadBytes)
{
    for (int i = 0; i < 64; i++)
    {
        fxPaths[i] = loadBytes[i * 6 + 0];
        levels[i] = loadBytes[i * 6 + 1];
        pannings[i] = loadBytes[i * 6 + 2];
        iLevels[i] = loadBytes[i * 6 + 3];
        iOutputs[i] = loadBytes[i * 6 + 4];
        sendLevels[i] = loadBytes[i * 6 + 5];
    }
}

ApsMixer::ApsMixer(std::vector<std::shared_ptr<StereoMixer>> &smcs,
                   std::vector<std::shared_ptr<IndivFxMixer>> &ifmcs)
{
    for (int i = 0; i < 64; i++)
    {
        auto mixerChannel = smcs[i];
        auto indivFxMixerChannel = ifmcs[i];
        saveBytes[i * 6 + 0] = (int8_t)indivFxMixerChannel->getFxPath();
        saveBytes[i * 6 + 1] = (int8_t)mixerChannel->getLevel();
        saveBytes[i * 6 + 2] = (int8_t)mixerChannel->getPanning();
        saveBytes[i * 6 + 3] =
            (int8_t)indivFxMixerChannel->getVolumeIndividualOut();
        saveBytes[i * 6 + 4] = (int8_t)indivFxMixerChannel->getOutput();
        saveBytes[i * 6 + 5] = (int8_t)indivFxMixerChannel->getFxSendLevel();
    }
}

StereoMixer ApsMixer::getStereoMixerChannel(int noteIndex)
{
    StereoMixer result;
    result.setLevel(getLevel(noteIndex));
    result.setPanning(getPanning(noteIndex));
    return result;
}

IndivFxMixer ApsMixer::getIndivFxMixerChannel(int noteIndex)
{
    IndivFxMixer result;
    result.setVolumeIndividualOut(getIndividualLevel(noteIndex));
    result.setOutput(getIndividualOutput(noteIndex));
    result.setFxSendLevel(getSendLevel(noteIndex));
    result.setFxPath(getFxPath(noteIndex));
    return result;
}

int ApsMixer::getFxPath(int noteIndex) const
{
    return fxPaths[noteIndex];
}

int ApsMixer::getLevel(int noteIndex) const
{
    return levels[noteIndex];
}

int ApsMixer::getPanning(int noteIndex) const
{
    return pannings[noteIndex];
}

int ApsMixer::getIndividualLevel(int noteIndex) const
{
    return iLevels[noteIndex];
}

int ApsMixer::getIndividualOutput(int noteIndex) const
{
    return iOutputs[noteIndex];
}

int ApsMixer::getSendLevel(int noteIndex) const
{
    return sendLevels[noteIndex];
}

std::vector<char> ApsMixer::getBytes()
{
    return saveBytes;
}
