#include "file/aps/ApsMixer.hpp"

#include "engine/StereoMixer.hpp"
#include "engine/IndivFxMixer.hpp"

using namespace mpc::file::aps;
using namespace mpc::performance;

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

ApsMixer::ApsMixer(
    const std::vector<std::shared_ptr<engine::StereoMixer>> &stereoMixer,
    const std::vector<std::shared_ptr<engine::IndivFxMixer>> &indivFxMixer)
{
    for (int i = 0; i < 64; i++)
    {
        const auto mixerChannel = stereoMixer[i];
        const auto indivFxMixerChannel = indivFxMixer[i];
        saveBytes[i * 6 + 0] =
            static_cast<int8_t>(indivFxMixerChannel->getFxPath());
        saveBytes[i * 6 + 1] = static_cast<int8_t>(mixerChannel->getLevel());
        saveBytes[i * 6 + 2] = static_cast<int8_t>(mixerChannel->getPanning());
        saveBytes[i * 6 + 3] =
            static_cast<int8_t>(indivFxMixerChannel->getVolumeIndividualOut());
        saveBytes[i * 6 + 4] =
            static_cast<int8_t>(indivFxMixerChannel->getOutput());
        saveBytes[i * 6 + 5] =
            static_cast<int8_t>(indivFxMixerChannel->getFxSendLevel());
    }
}

StereoMixer ApsMixer::getStereoMixerChannel(const int noteIndex) const
{
    StereoMixer result;
    result.level = DrumMixerLevel(getLevel(noteIndex));
    result.panning = DrumMixerPanning(getPanning(noteIndex));
    return result;
}

IndivFxMixer ApsMixer::getIndivFxMixerChannel(const int noteIndex) const
{
    IndivFxMixer result;
    result.individualOutLevel = DrumMixerLevel(getIndividualLevel(noteIndex));
    result.individualOutput =
        DrumMixerIndividualOutput(getIndividualOutput(noteIndex));
    result.fxSendLevel = DrumMixerLevel(getSendLevel(noteIndex));
    result.fxPath = DrumMixerIndividualFxPath(getFxPath(noteIndex));
    return result;
}

int ApsMixer::getFxPath(const int noteIndex) const
{
    return fxPaths[noteIndex];
}

int ApsMixer::getLevel(const int noteIndex) const
{
    return levels[noteIndex];
}

int ApsMixer::getPanning(const int noteIndex) const
{
    return pannings[noteIndex];
}

int ApsMixer::getIndividualLevel(const int noteIndex) const
{
    return iLevels[noteIndex];
}

int ApsMixer::getIndividualOutput(const int noteIndex) const
{
    return iOutputs[noteIndex];
}

int ApsMixer::getSendLevel(const int noteIndex) const
{
    return sendLevels[noteIndex];
}

std::vector<char> ApsMixer::getBytes()
{
    return saveBytes;
}
