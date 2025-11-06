#include "sampler/VoiceOverlapMode.hpp"
#include "file/aps/ApsNoteParameters.hpp"

#include "sampler/NoteParameters.hpp"

#include "file/ByteUtil.hpp"

#include <cassert>

using namespace mpc::file::aps;

ApsNoteParameters::ApsNoteParameters(const std::vector<char> &loadBytes)
{
    soundIndex =
        loadBytes[0] == '\xff' ? -1 : static_cast<unsigned char>(loadBytes[0]);
    assert(soundIndex >= -1);
    soundGenerationMode = loadBytes[2];
    velocityRangeLower = loadBytes[3];
    alsoPlay1 = loadBytes[4] == 0 ? 34 : loadBytes[4];
    velocityRangeUpper = loadBytes[5];
    alsoPlay2 = loadBytes[6] == 0 ? 34 : loadBytes[6];
    voiceOverlapMode = static_cast<sampler::VoiceOverlapMode>(loadBytes[7]);
    mute1 = loadBytes[8] == 0 ? 34 : loadBytes[8];
    mute2 = loadBytes[9] == 0 ? 34 : loadBytes[9];
    auto buf = std::vector<char>{loadBytes[10], loadBytes[11]};
    tune = ByteUtil::bytes2short(buf);
    attack = loadBytes[12];
    decay = loadBytes[13];
    decayMode = loadBytes[14];
    cutoffFrequency = loadBytes[15];
    resonance = loadBytes[16];
    filterAttack = loadBytes[17];
    filterDecay = loadBytes[18];
    filterEnvelopeAmount = loadBytes[19];
    velocityToLevel = loadBytes[20];
    velocityToAttack = loadBytes[21];
    velocityToStart = loadBytes[22];
    velocityToFilterFrequency = loadBytes[23];
    sliderParameter = loadBytes[24];
    velocityToPitch = loadBytes[25];
}

ApsNoteParameters::ApsNoteParameters(mpc::sampler::NoteParameters *np)
{
    saveBytes[0] = np->getSoundIndex() == -1 ? '\xff' : np->getSoundIndex();
    saveBytes[1] = np->getSoundIndex() == -1 ? '\xff' : '\x00';
    saveBytes[2] = np->getSoundGenerationMode();
    saveBytes[3] = np->getVelocityRangeLower();
    saveBytes[4] = np->getOptionalNoteA() == 34 ? 0 : np->getOptionalNoteA();
    saveBytes[5] = np->getVelocityRangeUpper();
    saveBytes[6] = np->getOptionalNoteB() == 34 ? 0 : np->getOptionalNoteB();
    saveBytes[7] = static_cast<int>(np->getVoiceOverlapMode());
    saveBytes[8] = np->getMuteAssignA() == 34 ? 0 : np->getMuteAssignA();
    saveBytes[9] = np->getMuteAssignB() == 34 ? 0 : np->getMuteAssignB();
    auto buf = ByteUtil::ushort2bytes(np->getTune());
    saveBytes[10] = buf[0];
    saveBytes[11] = buf[1];
    saveBytes[12] = np->getAttack();
    saveBytes[13] = np->getDecay();
    saveBytes[14] = np->getDecayMode();
    saveBytes[15] = np->getFilterFrequency();
    saveBytes[16] = np->getFilterResonance();
    saveBytes[17] = np->getFilterAttack();
    saveBytes[18] = np->getFilterDecay();
    saveBytes[19] = np->getFilterEnvelopeAmount();
    saveBytes[20] = np->getVeloToLevel();
    saveBytes[21] = np->getVelocityToAttack();
    saveBytes[22] = np->getVelocityToStart();
    saveBytes[23] = np->getVelocityToFilterFrequency();
    saveBytes[24] = np->getSliderParameterNumber();
    saveBytes[25] = np->getVelocityToPitch();
}

int ApsNoteParameters::getSoundIndex() const
{
    return soundIndex;
}

mpc::sampler::VoiceOverlapMode ApsNoteParameters::getVoiceOverlapMode() const
{
    return voiceOverlapMode;
}

int ApsNoteParameters::getTune() const
{
    return tune;
}

int ApsNoteParameters::getDecayMode() const
{
    return decayMode;
}

int ApsNoteParameters::getSoundGenerationMode() const
{
    return soundGenerationMode;
}

int ApsNoteParameters::getVelocityRangeLower() const
{
    return velocityRangeLower;
}

int ApsNoteParameters::getAlsoPlay1() const
{
    return alsoPlay1;
}

int ApsNoteParameters::getVelocityRangeUpper() const
{
    return velocityRangeUpper;
}

int ApsNoteParameters::getAlsoPlay2() const
{
    return alsoPlay2;
}

int ApsNoteParameters::getMute1() const
{
    return mute1;
}

int ApsNoteParameters::getMute2() const
{
    return mute2;
}

int ApsNoteParameters::getAttack() const
{
    return attack;
}

int ApsNoteParameters::getDecay() const
{
    return decay;
}

int ApsNoteParameters::getCutoffFrequency() const
{
    return cutoffFrequency;
}

int ApsNoteParameters::getResonance() const
{
    return resonance;
}

int ApsNoteParameters::getVelocityToFilterAttack() const
{
    return filterAttack;
}

int ApsNoteParameters::getVelocityToFilterDecay() const
{
    return filterDecay;
}

int ApsNoteParameters::getVelocityToFilterAmount() const
{
    return filterEnvelopeAmount;
}

int ApsNoteParameters::getVelocityToLevel() const
{
    return velocityToLevel;
}

int ApsNoteParameters::getVelocityToAttack() const
{
    return velocityToAttack;
}

int ApsNoteParameters::getVelocityToStart() const
{
    return velocityToStart;
}

int ApsNoteParameters::getVelocityToFilterFrequency() const
{
    return velocityToFilterFrequency;
}

int ApsNoteParameters::getSliderParameter() const
{
    return sliderParameter;
}

int ApsNoteParameters::getVelocityToPitch() const
{
    return velocityToPitch;
}

std::vector<char> ApsNoteParameters::getBytes()
{
    return saveBytes;
}
