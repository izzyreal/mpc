#include "sampler/NoteParameters.hpp"

#include "IntTypes.hpp"

#include "engine/IndivFxMixer.hpp"
#include "engine/StereoMixer.hpp"

using namespace mpc::sampler;

NoteParameters::NoteParameters(
    const int index, const std::function<ProgramIndex()> &getProgramIndex,
    GetNoteParametersFn &getSnapshot,
    const std::function<void(performance::PerformanceMessage &&)> &dispatch)
    : index(index), getProgramIndex(getProgramIndex),
      getSnapshot(std::move(getSnapshot)), dispatch(dispatch)
{
    stereoMixer = std::make_shared<engine::StereoMixer>(
        [this]
        {
            return this->getSnapshot().stereoMixer;
        },
        []
        {
            return NoDrumBusIndex;
        },
        getProgramIndex,
        [this]
        {
            return DrumNoteNumber(this->index + MinDrumNoteNumber);
        },
        dispatch);
    indivFxMixer = std::make_shared<engine::IndivFxMixer>(
        [this]
        {
            return this->getSnapshot().indivFxMixer;
        },
        []
        {
            return NoDrumBusIndex;
        },
        getProgramIndex,
        [this]
        {
            return DrumNoteNumber(this->index + MinDrumNoteNumber);
        },
        dispatch);
}

mpc::DrumNoteNumber NoteParameters::getNumber() const
{
    return DrumNoteNumber(index + MinDrumNoteNumber);
}

int NoteParameters::getSoundIndex() const
{
    return getSnapshot().soundIndex;
}

int NoteParameters::getSoundGenerationMode() const
{
    return getSnapshot().soundGenerationMode;
}

void NoteParameters::setSoundIndex(const int i) const
{
    const auto programIndex = getProgramIndex();
    performance::UpdateNoteParameters msg{
        programIndex, DrumNoteNumber(index + MinDrumNoteNumber)};
    msg.int16_tMemberToUpdate = &performance::NoteParameters::soundIndex;
    msg.int16_tValue = std::clamp(i, -1, 256);
    dispatch(performance::PerformanceMessage(std::move(msg)));
}

int NoteParameters::getVelocityRangeLower() const
{
    return getSnapshot().velocityRangeLower;
}

int NoteParameters::getVelocityRangeUpper() const
{
    return getSnapshot().velocityRangeUpper;
}

mpc::DrumNoteNumber NoteParameters::getOptionalNoteA() const
{
    return getSnapshot().optionalNoteA;
}

mpc::DrumNoteNumber NoteParameters::getOptionalNoteB() const
{
    return getSnapshot().optionalNoteB;
}

VoiceOverlapMode NoteParameters::getVoiceOverlapMode() const
{
    return getSnapshot().voiceOverlapMode;
}

mpc::DrumNoteNumber NoteParameters::getMuteAssignA() const
{
    return getSnapshot().muteAssignA;
}

mpc::DrumNoteNumber NoteParameters::getMuteAssignB() const
{
    return getSnapshot().muteAssignB;
}

int NoteParameters::getTune() const
{
    return getSnapshot().tune;
}

int NoteParameters::getAttack() const
{
    return getSnapshot().attack;
}

int NoteParameters::getDecay() const
{
    return getSnapshot().decay;
}

int NoteParameters::getDecayMode() const
{
    return getSnapshot().decayMode;
}

int NoteParameters::getFilterFrequency() const
{
    return getSnapshot().filterFrequency;
}

int NoteParameters::getFilterResonance() const
{
    return getSnapshot().filterResonance;
}

int NoteParameters::getFilterAttack() const
{
    return getSnapshot().filterAttack;
}

int NoteParameters::getFilterDecay() const
{
    return getSnapshot().filterDecay;
}

int NoteParameters::getFilterEnvelopeAmount() const
{
    return getSnapshot().filterEnvelopeAmount;
}

int NoteParameters::getVeloToLevel() const
{
    return getSnapshot().velocityToLevel;
}

int NoteParameters::getVelocityToAttack() const
{
    return getSnapshot().velocityToAttack;
}

int NoteParameters::getVelocityToStart() const
{
    return getSnapshot().velocityToStart;
}

int NoteParameters::getVelocityToFilterFrequency() const
{
    return getSnapshot().velocityToFilterFrequency;
}

int NoteParameters::getSliderParameterNumber() const
{
    return getSnapshot().sliderParameterNumber;
}

int NoteParameters::getVelocityToPitch() const
{
    return getSnapshot().velocityToPitch;
}

std::shared_ptr<mpc::engine::StereoMixer> NoteParameters::getStereoMixer() const
{
    return stereoMixer;
}

std::shared_ptr<mpc::engine::IndivFxMixer>
NoteParameters::getIndivFxMixer() const
{
    return indivFxMixer;
}

void NoteParameters::setSoundGenMode(const int i) const
{
    performance::UpdateNoteParameters msg{
        getProgramIndex(), DrumNoteNumber(index + MinDrumNoteNumber)};
    msg.int8_tMemberToUpdate =
        &performance::NoteParameters::soundGenerationMode;
    msg.int8_tValue = std::clamp(i, 0, 3);
    dispatch(std::move(msg));
}

void NoteParameters::setVeloRangeLower(const int i) const
{
    performance::UpdateNoteParameters msg{
        getProgramIndex(), DrumNoteNumber(index + MinDrumNoteNumber)};
    msg.int8_tMemberToUpdate = &performance::NoteParameters::velocityRangeLower;
    msg.int8_tValue = std::clamp(i, 0, 126);
    dispatch(std::move(msg));
}

void NoteParameters::setVeloRangeUpper(const int i) const
{
    performance::UpdateNoteParameters msg{
        getProgramIndex(), DrumNoteNumber(index + MinDrumNoteNumber)};
    msg.int8_tMemberToUpdate = &performance::NoteParameters::velocityRangeUpper;
    msg.int8_tValue = std::clamp(i, 1, 127);
    dispatch(std::move(msg));
}

void NoteParameters::setOptionalNoteA(const DrumNoteNumber i) const
{
    performance::UpdateNoteParameters msg{
        getProgramIndex(), DrumNoteNumber(index + MinDrumNoteNumber)};
    msg.drumNoteMemberToUpdate = &performance::NoteParameters::optionalNoteA;
    msg.drumNoteValue = i;
    dispatch(std::move(msg));
}

void NoteParameters::setOptionalNoteB(const DrumNoteNumber i) const
{
    performance::UpdateNoteParameters msg{
        getProgramIndex(), DrumNoteNumber(index + MinDrumNoteNumber)};
    msg.drumNoteMemberToUpdate = &performance::NoteParameters::optionalNoteB;
    msg.drumNoteValue = i;
    dispatch(std::move(msg));
}

void NoteParameters::setMuteAssignA(const DrumNoteNumber i) const
{
    performance::UpdateNoteParameters msg{
        getProgramIndex(), DrumNoteNumber(index + MinDrumNoteNumber)};
    msg.drumNoteMemberToUpdate = &performance::NoteParameters::muteAssignA;
    msg.drumNoteValue = i;
    dispatch(std::move(msg));
}

void NoteParameters::setMuteAssignB(const DrumNoteNumber i) const
{
    performance::UpdateNoteParameters msg{
        getProgramIndex(), DrumNoteNumber(index + MinDrumNoteNumber)};
    msg.drumNoteMemberToUpdate = &performance::NoteParameters::muteAssignB;
    msg.drumNoteValue = i;
    dispatch(std::move(msg));
}

void NoteParameters::setTune(const int i) const
{
    performance::UpdateNoteParameters msg{
        getProgramIndex(), DrumNoteNumber(index + MinDrumNoteNumber)};
    msg.int8_tMemberToUpdate = &performance::NoteParameters::tune;
    msg.int8_tValue = std::clamp(i, -120, 120);
    dispatch(std::move(msg));
}

void NoteParameters::setAttack(const int i) const
{
    performance::UpdateNoteParameters msg{
        getProgramIndex(), DrumNoteNumber(index + MinDrumNoteNumber)};
    msg.int8_tMemberToUpdate = &performance::NoteParameters::attack;
    msg.int8_tValue = std::clamp(i, 0, 100);
    dispatch(std::move(msg));
}

void NoteParameters::setDecay(const int i) const
{
    performance::UpdateNoteParameters msg{
        getProgramIndex(), DrumNoteNumber(index + MinDrumNoteNumber)};
    msg.int8_tMemberToUpdate = &performance::NoteParameters::decay;
    msg.int8_tValue = std::clamp(i, 0, 100);
    dispatch(std::move(msg));
}

void NoteParameters::setDecayMode(const int i) const
{
    performance::UpdateNoteParameters msg{
        getProgramIndex(), DrumNoteNumber(index + MinDrumNoteNumber)};
    msg.int8_tMemberToUpdate = &performance::NoteParameters::decayMode;
    msg.int8_tValue = std::clamp(i, 0, 1);
    dispatch(std::move(msg));
}

void NoteParameters::setFilterFrequency(const int i) const
{
    performance::UpdateNoteParameters msg{
        getProgramIndex(), DrumNoteNumber(index + MinDrumNoteNumber)};
    msg.int8_tMemberToUpdate = &performance::NoteParameters::filterFrequency;
    msg.int8_tValue = std::clamp(i, 0, 100);
    dispatch(std::move(msg));
}

void NoteParameters::setFilterResonance(const int i) const
{
    performance::UpdateNoteParameters msg{
        getProgramIndex(), DrumNoteNumber(index + MinDrumNoteNumber)};
    msg.int8_tMemberToUpdate = &performance::NoteParameters::filterResonance;
    msg.int8_tValue = std::clamp(i, 0, 15);
    dispatch(std::move(msg));
}

void NoteParameters::setFilterAttack(const int i) const
{
    performance::UpdateNoteParameters msg{
        getProgramIndex(), DrumNoteNumber(index + MinDrumNoteNumber)};
    msg.int8_tMemberToUpdate = &performance::NoteParameters::filterAttack;
    msg.int8_tValue = std::clamp(i, 0, 100);
    dispatch(std::move(msg));
}

void NoteParameters::setFilterDecay(const int i) const
{
    performance::UpdateNoteParameters msg{
        getProgramIndex(), DrumNoteNumber(index + MinDrumNoteNumber)};
    msg.int8_tMemberToUpdate = &performance::NoteParameters::filterDecay;
    msg.int8_tValue = std::clamp(i, 0, 100);
    dispatch(std::move(msg));
}

void NoteParameters::setFilterEnvelopeAmount(const int i) const
{
    performance::UpdateNoteParameters msg{
        getProgramIndex(), DrumNoteNumber(index + MinDrumNoteNumber)};
    msg.int8_tMemberToUpdate =
        &performance::NoteParameters::filterEnvelopeAmount;
    msg.int8_tValue = std::clamp(i, 0, 100);
    dispatch(std::move(msg));
}

void NoteParameters::setVeloToLevel(const int i) const
{
    performance::UpdateNoteParameters msg{
        getProgramIndex(), DrumNoteNumber(index + MinDrumNoteNumber)};
    msg.int8_tMemberToUpdate = &performance::NoteParameters::velocityToLevel;
    msg.int8_tValue = std::clamp(i, 0, 100);
    dispatch(std::move(msg));
}

void NoteParameters::setVelocityToAttack(const int i) const
{
    performance::UpdateNoteParameters msg{
        getProgramIndex(), DrumNoteNumber(index + MinDrumNoteNumber)};
    msg.int8_tMemberToUpdate = &performance::NoteParameters::velocityToAttack;
    msg.int8_tValue = std::clamp(i, 0, 100);
    dispatch(std::move(msg));
}

void NoteParameters::setVelocityToStart(const int i) const
{
    performance::UpdateNoteParameters msg{
        getProgramIndex(), DrumNoteNumber(index + MinDrumNoteNumber)};
    msg.int8_tMemberToUpdate = &performance::NoteParameters::velocityToStart;
    msg.int8_tValue = std::clamp(i, 0, 100);
    dispatch(std::move(msg));
}

void NoteParameters::setVelocityToFilterFrequency(const int i) const
{
    performance::UpdateNoteParameters msg{
        getProgramIndex(), DrumNoteNumber(index + MinDrumNoteNumber)};
    msg.int8_tMemberToUpdate =
        &performance::NoteParameters::velocityToFilterFrequency;
    msg.int8_tValue = std::clamp(i, 0, 100);
    dispatch(std::move(msg));
}

void NoteParameters::setSliderParameterNumber(const int i) const
{
    performance::UpdateNoteParameters msg{
        getProgramIndex(), DrumNoteNumber(index + MinDrumNoteNumber)};
    msg.int8_tMemberToUpdate =
        &performance::NoteParameters::sliderParameterNumber;
    msg.int8_tValue = std::clamp(i, 0, 3);
    dispatch(std::move(msg));
}

void NoteParameters::setVelocityToPitch(const int i) const
{
    performance::UpdateNoteParameters msg{
        getProgramIndex(), DrumNoteNumber(index + MinDrumNoteNumber)};
    msg.int8_tMemberToUpdate = &performance::NoteParameters::velocityToPitch;
    msg.int8_tValue = std::clamp(i, -120, 120);
    dispatch(std::move(msg));
}

void NoteParameters::setVoiceOverlapMode(const VoiceOverlapMode m) const
{
    if (m == VoiceOverlapMode::MONO || m == VoiceOverlapMode::NOTE_OFF ||
        m == VoiceOverlapMode::POLY)
    {
        performance::UpdateNoteParameters msg{
            getProgramIndex(), DrumNoteNumber(index + MinDrumNoteNumber)};
        msg.voiceOverlapModeMemberToUpdate =
            &performance::NoteParameters::voiceOverlapMode;
        msg.voiceOverlapMode = m;
        dispatch(std::move(msg));
    }
}
