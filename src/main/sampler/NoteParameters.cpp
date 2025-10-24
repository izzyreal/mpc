#include <algorithm>
#include "sampler/NoteParameters.hpp"

#include <cassert>

using namespace mpc::sampler;
using namespace mpc::engine;

NoteParameters::NoteParameters(const int index)
    : index(index)
{
	decay = 5;
	filterFrequency = 100; 
	velocityRangeLower = 44;
	velocityRangeUpper = 88;
	velocityToLevel = 100;
	muteAssignA = 34;
	muteAssignB = 34;
	optionalNoteA = 34;
	optionalNoteB = 34;
}

int NoteParameters::getSoundIndex()
{
    return soundIndex;
}

void NoteParameters::setSoundIndex(int i)
{
    assert(i >= -1);
    soundIndex = i;
}

void NoteParameters::setSoundGenMode(int i)
{
    soundGenerationMode = std::clamp(i, 0, 3);
}

int NoteParameters::getSoundGenerationMode()
{
    return soundGenerationMode;
}

void NoteParameters::setVeloRangeLower(int i)
{
    velocityRangeLower = std::clamp(i, 0, 126);

    if (velocityRangeUpper <= velocityRangeLower)
    {
        setVeloRangeUpper(velocityRangeLower + 1);
    }
}

void NoteParameters::setVeloRangeUpper(int i)
{
	velocityRangeUpper = std::clamp(i, 1, 127);

    if (velocityRangeLower >= velocityRangeUpper)
    {
        setVeloRangeLower(velocityRangeUpper - 1);
    }
}

int NoteParameters::getVelocityRangeLower()
{
    return velocityRangeLower;
}

void NoteParameters::setOptNoteA(int i)
{
	optionalNoteA = std::clamp(i, 34, 98);
}

int NoteParameters::getOptionalNoteA()
{
    return optionalNoteA;
}

int NoteParameters::getVelocityRangeUpper()
{
    return velocityRangeUpper;
}

void NoteParameters::setOptionalNoteB(int i)
{
	optionalNoteB = std::clamp(i, 34, 98);
}

int NoteParameters::getOptionalNoteB()
{
    return optionalNoteB;
}

VoiceOverlapMode NoteParameters::getVoiceOverlapMode()
{
    return voiceOverlapMode;
}

void NoteParameters::setVoiceOverlapMode(const VoiceOverlapMode voiceOverlapModeToUse)
{
    if (voiceOverlapMode != VoiceOverlapMode::MONO &&
            voiceOverlapMode != VoiceOverlapMode::NOTE_OFF &&
            voiceOverlapMode != VoiceOverlapMode::POLY)
    {
        return;
    }

    voiceOverlapMode = voiceOverlapModeToUse;
 }

void NoteParameters::setMuteAssignA(int i)
{
    muteAssignA = std::clamp(i, 34, 98);
}

int NoteParameters::getMuteAssignA()
{
    return muteAssignA;
}

void NoteParameters::setMuteAssignB(int i)
{
    muteAssignB = std::clamp(i, 34, 98);
}

int NoteParameters::getMuteAssignB()
{
    return muteAssignB;
}

void NoteParameters::setTune(int i)
{
    tune = std::clamp(i, -240, 240);
}

int NoteParameters::getTune()
{
    return tune;
}

void NoteParameters::setAttack(int i)
{
    attack = std::clamp(i, 0, 100);
}

int NoteParameters::getAttack()
{
    return attack;
}

void NoteParameters::setDecay(int i)
{
    decay = std::clamp(i, 0, 100);
}

int NoteParameters::getDecay()
{
    return decay;
}

void NoteParameters::setDecayMode(int i)
{
    decayMode = std::clamp(i, 0, 1);
}

int NoteParameters::getDecayMode()
{
    return decayMode;
}

void NoteParameters::setFilterFrequency(int i)
{
    filterFrequency = std::clamp(i, 0, 100);
}

int NoteParameters::getFilterFrequency()
{
    return filterFrequency;
}

void NoteParameters::setFilterResonance(int i)
{
    filterResonance = std::clamp(i, 0, 15);
}

int NoteParameters::getFilterResonance()
{
    return filterResonance;
}

void NoteParameters::setFilterAttack(int i)
{
    filterAttack = std::clamp(i, 0, 100);
}

int NoteParameters::getFilterAttack()
{
    return filterAttack;
}

void NoteParameters::setFilterDecay(int i)
{
    filterDecay = std::clamp(i, 0, 100);
}

int NoteParameters::getFilterDecay()
{
    return filterDecay;
}

void NoteParameters::setFilterEnvelopeAmount(int i)
{
    filterEnvelopeAmount = std::clamp(i, 0, 100);
}

int NoteParameters::getFilterEnvelopeAmount()
{
    return filterEnvelopeAmount;
}

void NoteParameters::setVeloToLevel(int i)
{
    velocityToLevel = std::clamp(i, 0, 100);
}

int NoteParameters::getVeloToLevel()
{
    return velocityToLevel;
}

void NoteParameters::setVelocityToAttack(int i)
{
    velocityToAttack = std::clamp(i, 0, 100);
}

int NoteParameters::getVelocityToAttack()
{
    return velocityToAttack;
}

void NoteParameters::setVelocityToStart(int i)
{
    velocityToStart = std::clamp(i, 0, 100);
}

int NoteParameters::getVelocityToStart()
{
    return velocityToStart;
}

void NoteParameters::setVelocityToFilterFrequency(int i)
{
    velocityToFilterFrequency = std::clamp(i, 0, 100);
}

int NoteParameters::getVelocityToFilterFrequency()
{
    return velocityToFilterFrequency;
}

void NoteParameters::setSliderParameterNumber(int i)
{
    sliderParameterNumber = std::clamp(i, 0, 3);
}

int NoteParameters::getSliderParameterNumber()
{
    return sliderParameterNumber;
}

void NoteParameters::setVelocityToPitch(int i)
{
    velocityToPitch = std::clamp(i, -120, 120);
}

int NoteParameters::getVelocityToPitch()
{
    return velocityToPitch;
}

NoteParameters* NoteParameters::clone(const int newIndex)
{
	auto res = new NoteParameters(newIndex);
	res->setAttack(attack);
	res->setDecay(decay);
	res->setDecayMode(decayMode);
	res->setFilterAttack(filterAttack);
	res->setFilterDecay(filterDecay);
	res->setFilterEnvelopeAmount(filterEnvelopeAmount);
	res->setFilterFrequency(filterFrequency);
	res->setFilterResonance(filterResonance);
	res->setMuteAssignA(muteAssignA);
	res->setMuteAssignB(muteAssignB);
	res->setOptNoteA(optionalNoteA);
	res->setOptionalNoteB(optionalNoteB);
	res->setSliderParameterNumber(sliderParameterNumber);
	res->setSoundGenMode(soundGenerationMode);
	res->setSoundIndex(soundIndex);
	res->setTune(tune);
	res->setVelocityToAttack(velocityToAttack);
	res->setVelocityToFilterFrequency(velocityToFilterFrequency);
	res->setVelocityToPitch(velocityToPitch);
	res->setVeloToLevel(velocityToLevel);
	res->setVelocityToStart(velocityToStart);
	res->setVeloRangeLower(velocityRangeLower);
	res->setVeloRangeUpper(velocityRangeUpper);
	res->setVoiceOverlapMode(voiceOverlapMode);
	return res;
}

int NoteParameters::getNumber()
{
    return index + 35;
}

std::shared_ptr<StereoMixer> NoteParameters::getStereoMixerChannel()
{
    return stereoMixerChannel;
}

std::shared_ptr<IndivFxMixer> NoteParameters::getIndivFxMixerChannel()
{
    return indivFxMixerChannel;
}
