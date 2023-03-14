#include <sampler/NoteParameters.hpp>

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
    soundIndex = i;
}

void NoteParameters::setSoundGenMode(int i)
{
    if (soundGenerationMode == i)
        return;

    if (i < 0 || i > 3)
        return;

    soundGenerationMode = i;
  }

int NoteParameters::getSoundGenerationMode()
{
    return soundGenerationMode;
}

void NoteParameters::setVeloRangeLower(int i)
{
    if (velocityRangeLower == i)
        return;

    if (i < 0 || i > 126)
        return;

    if (i > velocityRangeUpper - 2)
        setVeloRangeUpper(i - 1);

    velocityRangeLower = i;
 }

int NoteParameters::getVelocityRangeLower()
{
    return velocityRangeLower;
}

void NoteParameters::setOptNoteA(int i)
{
	if (optionalNoteA == i)
        return;

	if (i < 34 || i > 98)
        return;

	optionalNoteA = i;
}

int NoteParameters::getOptionalNoteA()
{
    return optionalNoteA;
}

void NoteParameters::setVeloRangeUpper(int i)
{
	if (velocityRangeUpper == i)
		return;

	if (i < velocityRangeLower + 1 || i > 127)
		return;

	velocityRangeUpper = i;
}

int NoteParameters::getVelocityRangeUpper()
{
    return velocityRangeUpper;
}

void NoteParameters::setOptionalNoteB(int i)
{
	if (optionalNoteB == i)
		return;

	if (i < 34 || i > 98)
		return;

	optionalNoteB = i;
}

int NoteParameters::getOptionalNoteB()
{
    return optionalNoteB;
}

int NoteParameters::getVoiceOverlap()
{
    return voiceOverlap;
}

void NoteParameters::setVoiceOverlap(int i)
{
    if (i < 0 || i > 2)
        return;

    voiceOverlap = i;
 }

void NoteParameters::setMuteAssignA(int i)
{
    if (i < 34 || i > 98)
        return;

    muteAssignA = i;
}

int NoteParameters::getMuteAssignA()
{
    return muteAssignA;
}

void NoteParameters::setMuteAssignB(int i)
{
    if (i < 34 || i > 98)
        return;

    muteAssignB = i;
}

int NoteParameters::getMuteAssignB()
{
    return muteAssignB;
}

void NoteParameters::setTune(int i)
{
    if (tune == i)
        return;

    if (i < -240)
        i = -240;
    else if (i > 240)
        i = 240;

    tune = i;
}

int NoteParameters::getTune()
{
    return tune;
}

void NoteParameters::setAttack(int i)
{
    if(attack == i)
        return;

    if(i < 0 || i > 100)
        return;

    attack = i;
}

int NoteParameters::getAttack()
{
    return attack;
}

void NoteParameters::setDecay(int i)
{
    if (decay == i)
        return;

    if (i < 0 || i > 100)
        return;

    decay = i;
}

int NoteParameters::getDecay()
{
    return decay;
}

void NoteParameters::setDecayMode(int i)
{
    if (decayMode == i)
        return;

    if (i < 0 || i > 1)
        return;

    decayMode = i;
}

int NoteParameters::getDecayMode()
{
    return decayMode;
}

void NoteParameters::setFilterFrequency(int i)
{
    if (filterFrequency == i)
        return;

    if (i < 0 || i > 100)
        return;

    filterFrequency = i;
}

int NoteParameters::getFilterFrequency()
{
    return filterFrequency;
}

void NoteParameters::setFilterResonance(int i)
{
    if (filterResonance == i)
        return;

    if (i < 0 || i > 15)
        return;

    filterResonance = i;
}

int NoteParameters::getFilterResonance()
{
    return filterResonance;
}

void NoteParameters::setFilterAttack(int i)
{
    if (filterAttack == i)
        return;

    if (i < 0 || i > 100)
        return;

    filterAttack = i;
}

int NoteParameters::getFilterAttack()
{
    return filterAttack;
}

void NoteParameters::setFilterDecay(int i)
{
    if (filterDecay == i)
        return;

    if (i < 0 || i > 100)
        return;

    filterDecay = i;
}

int NoteParameters::getFilterDecay()
{
    return filterDecay;
}

void NoteParameters::setFilterEnvelopeAmount(int i)
{
    if (filterEnvelopeAmount == i)
        return;

    if (i < 0 || i > 100)
        return;

    filterEnvelopeAmount = i;
}

int NoteParameters::getFilterEnvelopeAmount()
{
    return filterEnvelopeAmount;
}

void NoteParameters::setVeloToLevel(int i)
{
    if (velocityToLevel == i)
        return;

    if (i < 0 || i > 100)
        return;

    velocityToLevel = i;
}

int NoteParameters::getVeloToLevel()
{
    return velocityToLevel;
}

void NoteParameters::setVelocityToAttack(int i)
{
    if (velocityToAttack == i)
        return;

    if (i < 0 || i > 100)
        return;

    velocityToAttack = i;
}

int NoteParameters::getVelocityToAttack()
{
    return velocityToAttack;
}

void NoteParameters::setVelocityToStart(int i)
{
    if (velocityToStart == i)
        return;

    if (i < 0 || i > 100)
        return;

    velocityToStart = i;
}

int NoteParameters::getVelocityToStart()
{
    return velocityToStart;
}

void NoteParameters::setVelocityToFilterFrequency(int i)
{
    if (velocityToFilterFrequency == i)
        return;

    if (i < 0 || i > 100)
        return;

    velocityToFilterFrequency = i;
}

int NoteParameters::getVelocityToFilterFrequency()
{
    return velocityToFilterFrequency;
}

void NoteParameters::setSliderParameterNumber(int i)
{
    if (sliderParameterNumber == i)
        return;

    if (i < 0 || i > 3)
        return;

    sliderParameterNumber = i;
}

int NoteParameters::getSliderParameterNumber()
{
    return sliderParameterNumber;
}

void NoteParameters::setVelocityToPitch(int i)
{
    if (velocityToPitch == i)
        return;

    if (i < -120 || i > 120)
        return;

    velocityToPitch = i;
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
	res->setVoiceOverlap(voiceOverlap);
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
