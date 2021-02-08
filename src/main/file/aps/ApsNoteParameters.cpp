#include <file/aps/ApsNoteParameters.hpp>

#include <sampler/NoteParameters.hpp>

#include <file/ByteUtil.hpp>

using namespace mpc::file::aps;
using namespace std;

ApsNoteParameters::ApsNoteParameters(const vector<char>& loadBytes)
{
	soundNumber = loadBytes[0] == '\xff' ? -1 : loadBytes[0];
	soundGenerationMode = loadBytes[2];
	velocityRangeLower = loadBytes[3];
	alsoPlay1 = loadBytes[4] == 0 ? 34 : loadBytes[4];
	velocityRangeUpper = loadBytes[5];
	alsoPlay2 = loadBytes[6] == 0 ? 34 : loadBytes[6];
	voiceOverlap = loadBytes[7];
	mute1 = loadBytes[8] == 0 ? 34 : loadBytes[8];
	mute2 = loadBytes[9] == 0 ? 34 : loadBytes[9];
	auto buf = vector<char>{ loadBytes[10], loadBytes[11] };
	tune = moduru::file::ByteUtil::bytes2short(buf);
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

ApsNoteParameters::ApsNoteParameters(mpc::sampler::NoteParameters* np)
{
	saveBytes[0] = np->getSoundIndex() == -1 ? '\xff' : np->getSoundIndex();
	saveBytes[1] = np->getSoundIndex() == -1 ? '\xff' : '\x00';
	saveBytes[2] = np->getSoundGenerationMode();
	saveBytes[3] = np->getVelocityRangeLower();
	saveBytes[4] = np->getOptionalNoteA() == 34 ? 0 : np->getOptionalNoteA();
	saveBytes[5] = np->getVelocityRangeUpper();
	saveBytes[6] = np->getOptionalNoteB() == 34 ? 0 : np->getOptionalNoteB();
	saveBytes[7] = np->getVoiceOverlap();
	saveBytes[8] = np->getMuteAssignA() == 34 ? 0 : np->getMuteAssignA();
	saveBytes[9] = np->getMuteAssignB() == 34 ? 0 : np->getMuteAssignB();
	auto buf = moduru::file::ByteUtil::ushort2bytes(np->getTune());
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

int ApsNoteParameters::getSoundNumber()
{
    return soundNumber;
}

int ApsNoteParameters::getVoiceOverlap()
{
    return voiceOverlap;
}

int ApsNoteParameters::getTune()
{
    return tune;
}

int ApsNoteParameters::getDecayMode()
{
    return decayMode;
}

int ApsNoteParameters::getSoundGenerationMode()
{
    return soundGenerationMode;
}

int ApsNoteParameters::getVelocityRangeLower()
{
    return velocityRangeLower;
}

int ApsNoteParameters::getAlsoPlay1()
{
    return alsoPlay1;
}

int ApsNoteParameters::getVelocityRangeUpper()
{
    return velocityRangeUpper;
}

int ApsNoteParameters::getAlsoPlay2()
{
    return alsoPlay2;
}

int ApsNoteParameters::getMute1()
{
    return mute1;
}

int ApsNoteParameters::getMute2()
{
    return mute2;
}

int ApsNoteParameters::getAttack()
{
    return attack;
}

int ApsNoteParameters::getDecay()
{
    return decay;
}

int ApsNoteParameters::getCutoffFrequency()
{
    return cutoffFrequency;
}

int ApsNoteParameters::getResonance()
{
    return resonance;
}

int ApsNoteParameters::getVelocityToFilterAttack()
{
    return filterAttack;
}

int ApsNoteParameters::getVelocityToFilterDecay()
{
    return filterDecay;
}

int ApsNoteParameters::getVelocityToFilterAmount()
{
    return filterEnvelopeAmount;
}

int ApsNoteParameters::getVelocityToLevel()
{
    return velocityToLevel;
}

int ApsNoteParameters::getVelocityToAttack()
{
    return velocityToAttack;
}

int ApsNoteParameters::getVelocityToStart()
{
    return velocityToStart;
}

int ApsNoteParameters::getVelocityToFilterFrequency()
{
    return velocityToFilterFrequency;
}

int ApsNoteParameters::getSliderParameter()
{
    return sliderParameter;
}

int ApsNoteParameters::getVelocityToPitch()
{
    return velocityToPitch;
}

vector<char> ApsNoteParameters::getBytes()
{
    return saveBytes;
}
