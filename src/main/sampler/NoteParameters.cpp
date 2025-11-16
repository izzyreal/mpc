#include <algorithm>
#include "sampler/NoteParameters.hpp"

#include "IntTypes.hpp"

#include <cassert>

using namespace mpc::sampler;
using namespace mpc::engine;

NoteParameters::NoteParameters(const int index,
    const std::function<performance::NoteParameters()> &getSnapshot,
    const std::function<void(performance::PerformanceMessage &)> &dispatch)
        : index(index), getSnapshot(getSnapshot), dispatch(dispatch)
{
    decay = 5;
    filterFrequency = 100;
    velocityRangeLower = 44;
    velocityRangeUpper = 88;
    velocityToLevel = 100;
    muteAssignA = NoDrumNoteAssigned;
    muteAssignB = NoDrumNoteAssigned;
    optionalNoteA = NoDrumNoteAssigned;
    optionalNoteB = NoDrumNoteAssigned;
}

int NoteParameters::getSoundIndex() const {
    return soundIndex;
}

void NoteParameters::setSoundIndex(const int i) {
    assert(i >= -1);
    soundIndex = i;
}

void NoteParameters::setSoundGenMode(const int i) {
    soundGenerationMode = std::clamp(i, 0, 3);
}

int NoteParameters::getSoundGenerationMode() const {
    return soundGenerationMode;
}

void NoteParameters::setVeloRangeLower(const int i) {
    velocityRangeLower = std::clamp(i, 0, 126);

    if (velocityRangeUpper <= velocityRangeLower) {
        setVeloRangeUpper(velocityRangeLower + 1);
    }
}

void NoteParameters::setVeloRangeUpper(const int i) {
    velocityRangeUpper = std::clamp(i, 1, 127);

    if (velocityRangeLower >= velocityRangeUpper) {
        setVeloRangeLower(velocityRangeUpper - 1);
    }
}

int NoteParameters::getVelocityRangeLower() const {
    return velocityRangeLower;
}

void NoteParameters::setOptNoteA(const DrumNoteNumber i) {
    optionalNoteA = std::clamp(i, NoDrumNoteAssigned, MaxDrumNoteNumber);
}

mpc::DrumNoteNumber NoteParameters::getOptionalNoteA() const {
    return optionalNoteA;
}

int NoteParameters::getVelocityRangeUpper() const {
    return velocityRangeUpper;
}

void NoteParameters::setOptionalNoteB(const DrumNoteNumber i) {
    optionalNoteB = std::clamp(i, NoDrumNoteAssigned, MaxDrumNoteNumber);
}

mpc::DrumNoteNumber NoteParameters::getOptionalNoteB() const {
    return optionalNoteB;
}

VoiceOverlapMode NoteParameters::getVoiceOverlapMode() const {
    return voiceOverlapMode;
}

void NoteParameters::setVoiceOverlapMode(
    const VoiceOverlapMode voiceOverlapModeToUse) {
    if (voiceOverlapMode != VoiceOverlapMode::MONO &&
        voiceOverlapMode != VoiceOverlapMode::NOTE_OFF &&
        voiceOverlapMode != VoiceOverlapMode::POLY) {
        return;
    }

    voiceOverlapMode = voiceOverlapModeToUse;
}

void NoteParameters::setMuteAssignA(const DrumNoteNumber i) {
    muteAssignA = std::clamp(i, NoDrumNoteAssigned, MaxDrumNoteNumber);
}

mpc::DrumNoteNumber NoteParameters::getMuteAssignA() const {
    return muteAssignA;
}

void NoteParameters::setMuteAssignB(const DrumNoteNumber i) {
    muteAssignB = std::clamp(i, NoDrumNoteAssigned, MaxDrumNoteNumber);
}

mpc::DrumNoteNumber NoteParameters::getMuteAssignB() const {
    return muteAssignB;
}

void NoteParameters::setTune(const int i) {
    tune = std::clamp(i, -240, 240);
}

int NoteParameters::getTune() const {
    return tune;
}

void NoteParameters::setAttack(const int i) {
    attack = std::clamp(i, 0, 100);
}

int NoteParameters::getAttack() const {
    return attack;
}

void NoteParameters::setDecay(const int i) {
    decay = std::clamp(i, 0, 100);
}

int NoteParameters::getDecay() const {
    return decay;
}

void NoteParameters::setDecayMode(const int i) {
    decayMode = std::clamp(i, 0, 1);
}

int NoteParameters::getDecayMode() const {
    return decayMode;
}

void NoteParameters::setFilterFrequency(const int i) {
    filterFrequency = std::clamp(i, 0, 100);
}

int NoteParameters::getFilterFrequency() const {
    return filterFrequency;
}

void NoteParameters::setFilterResonance(const int i) {
    filterResonance = std::clamp(i, 0, 15);
}

int NoteParameters::getFilterResonance() const {
    return filterResonance;
}

void NoteParameters::setFilterAttack(const int i) {
    filterAttack = std::clamp(i, 0, 100);
}

int NoteParameters::getFilterAttack() const {
    return filterAttack;
}

void NoteParameters::setFilterDecay(const int i) {
    filterDecay = std::clamp(i, 0, 100);
}

int NoteParameters::getFilterDecay() const {
    return filterDecay;
}

void NoteParameters::setFilterEnvelopeAmount(const int i) {
    filterEnvelopeAmount = std::clamp(i, 0, 100);
}

int NoteParameters::getFilterEnvelopeAmount() const {
    return filterEnvelopeAmount;
}

void NoteParameters::setVeloToLevel(const int i) {
    velocityToLevel = std::clamp(i, 0, 100);
}

int NoteParameters::getVeloToLevel() const {
    return velocityToLevel;
}

void NoteParameters::setVelocityToAttack(const int i) {
    velocityToAttack = std::clamp(i, 0, 100);
}

int NoteParameters::getVelocityToAttack() const {
    return velocityToAttack;
}

void NoteParameters::setVelocityToStart(const int i) {
    velocityToStart = std::clamp(i, 0, 100);
}

int NoteParameters::getVelocityToStart() const {
    return velocityToStart;
}

void NoteParameters::setVelocityToFilterFrequency(const int i) {
    velocityToFilterFrequency = std::clamp(i, 0, 100);
}

int NoteParameters::getVelocityToFilterFrequency() const {
    return velocityToFilterFrequency;
}

void NoteParameters::setSliderParameterNumber(const int i) {
    sliderParameterNumber = std::clamp(i, 0, 3);
}

int NoteParameters::getSliderParameterNumber() const {
    return sliderParameterNumber;
}

void NoteParameters::setVelocityToPitch(const int i) {
    velocityToPitch = std::clamp(i, -120, 120);
}

int NoteParameters::getVelocityToPitch() const {
    return velocityToPitch;
}

NoteParameters *NoteParameters::clone(const int newIndex) const {
    const auto res = new NoteParameters(newIndex, getSnapshot, dispatch);
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

mpc::DrumNoteNumber NoteParameters::getNumber() const {
    return DrumNoteNumber(index + MinDrumNoteNumber);
}

std::shared_ptr<StereoMixer> NoteParameters::getStereoMixerChannel() {
    return stereoMixerChannel;
}

std::shared_ptr<IndivFxMixer> NoteParameters::getIndivFxMixerChannel() {
    return indivFxMixerChannel;
}
