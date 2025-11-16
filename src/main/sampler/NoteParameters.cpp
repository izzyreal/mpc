#include "sampler/NoteParameters.hpp"

#include "IntTypes.hpp"

#include "engine/IndivFxMixer.hpp"
#include "engine/StereoMixer.hpp"

using namespace mpc::sampler;

NoteParameters::NoteParameters(
    const int index, const std::function<ProgramIndex()> &getProgramIndex,
    const std::function<performance::NoteParameters()> &getSnapshot,
    const std::function<void(performance::PerformanceMessage &&)> &dispatch)
    : index(index), getProgramIndex(getProgramIndex), getSnapshot(getSnapshot),
      dispatch(dispatch)
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

NoteParameters *NoteParameters::clone(const int newIndex) const
{
    const auto res =
        new NoteParameters(newIndex, getProgramIndex, getSnapshot, dispatch);
    return res;
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
    performance::PerformanceMessage msg;
    performance::UpdateNoteParameters payload{
        getProgramIndex(), DrumNoteNumber(index + MinDrumNoteNumber)};
    payload.int16_tMemberToUpdate = &performance::NoteParameters::soundIndex;
    payload.int16_tValue = std::clamp(i, -1, 256);
    msg.payload = std::move(payload);
    dispatch(std::move(msg));
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

std::shared_ptr<mpc::engine::StereoMixer> NoteParameters::getStereoMixer()
{
    return stereoMixer;
}

std::shared_ptr<mpc::engine::IndivFxMixer> NoteParameters::getIndivFxMixer()
{
    return indivFxMixer;
}

void NoteParameters::setSoundGenMode(const int i) const
{
    performance::PerformanceMessage msg;
    performance::UpdateNoteParameters payload{
        getProgramIndex(), DrumNoteNumber(index + MinDrumNoteNumber)};
    payload.int8_tMemberToUpdate =
        &performance::NoteParameters::soundGenerationMode;
    payload.int8_tValue = std::clamp(i, 0, 3);
    msg.payload = std::move(payload);
    dispatch(std::move(msg));
}

void NoteParameters::setVeloRangeLower(const int i) const
{
    performance::PerformanceMessage msg;
    performance::UpdateNoteParameters payload{
        getProgramIndex(), DrumNoteNumber(index + MinDrumNoteNumber)};
    payload.int8_tMemberToUpdate =
        &performance::NoteParameters::velocityRangeLower;
    payload.int8_tValue = std::clamp(i, 0, 126);
    msg.payload = std::move(payload);
    dispatch(std::move(msg));
}

void NoteParameters::setVeloRangeUpper(const int i) const
{
    performance::PerformanceMessage msg;
    performance::UpdateNoteParameters payload{
        getProgramIndex(), DrumNoteNumber(index + MinDrumNoteNumber)};
    payload.int8_tMemberToUpdate =
        &performance::NoteParameters::velocityRangeUpper;
    payload.int8_tValue = std::clamp(i, 1, 127);
    msg.payload = std::move(payload);
    dispatch(std::move(msg));
}

void NoteParameters::setOptionalNoteA(const DrumNoteNumber i) const
{
    performance::PerformanceMessage msg;
    performance::UpdateNoteParameters payload{
        getProgramIndex(), DrumNoteNumber(index + MinDrumNoteNumber)};
    payload.drumNoteMemberToUpdate =
        &performance::NoteParameters::optionalNoteA;
    payload.drumNoteValue = i;
    msg.payload = std::move(payload);
    dispatch(std::move(msg));
}

void NoteParameters::setOptionalNoteB(const DrumNoteNumber i) const
{
    performance::PerformanceMessage msg;
    performance::UpdateNoteParameters payload{
        getProgramIndex(), DrumNoteNumber(index + MinDrumNoteNumber)};
    payload.drumNoteMemberToUpdate =
        &performance::NoteParameters::optionalNoteB;
    payload.drumNoteValue = i;
    msg.payload = std::move(payload);
    dispatch(std::move(msg));
}

void NoteParameters::setMuteAssignA(const DrumNoteNumber i) const
{
    performance::PerformanceMessage msg;
    performance::UpdateNoteParameters payload{
        getProgramIndex(), DrumNoteNumber(index + MinDrumNoteNumber)};
    payload.drumNoteMemberToUpdate = &performance::NoteParameters::muteAssignA;
    payload.drumNoteValue = i;
    msg.payload = std::move(payload);
    dispatch(std::move(msg));
}

void NoteParameters::setMuteAssignB(const DrumNoteNumber i) const
{
    performance::PerformanceMessage msg;
    performance::UpdateNoteParameters payload{
        getProgramIndex(), DrumNoteNumber(index + MinDrumNoteNumber)};
    payload.drumNoteMemberToUpdate = &performance::NoteParameters::muteAssignB;
    payload.drumNoteValue = i;
    msg.payload = std::move(payload);
    dispatch(std::move(msg));
}

void NoteParameters::setTune(const int i) const
{
    performance::PerformanceMessage msg;
    performance::UpdateNoteParameters payload{
        getProgramIndex(), DrumNoteNumber(index + MinDrumNoteNumber)};
    payload.int8_tMemberToUpdate = &performance::NoteParameters::tune;
    payload.int8_tValue = std::clamp(i, -120, 120);
    msg.payload = std::move(payload);
    dispatch(std::move(msg));
}

void NoteParameters::setAttack(const int i) const
{
    performance::PerformanceMessage msg;
    performance::UpdateNoteParameters payload{
        getProgramIndex(), DrumNoteNumber(index + MinDrumNoteNumber)};
    payload.int8_tMemberToUpdate = &performance::NoteParameters::attack;
    payload.int8_tValue = std::clamp(i, 0, 100);
    msg.payload = std::move(payload);
    dispatch(std::move(msg));
}

void NoteParameters::setDecay(const int i) const
{
    performance::PerformanceMessage msg;
    performance::UpdateNoteParameters payload{
        getProgramIndex(), DrumNoteNumber(index + MinDrumNoteNumber)};
    payload.int8_tMemberToUpdate = &performance::NoteParameters::decay;
    payload.int8_tValue = std::clamp(i, 0, 100);
    msg.payload = std::move(payload);
    dispatch(std::move(msg));
}

void NoteParameters::setDecayMode(const int i) const
{
    performance::PerformanceMessage msg;
    performance::UpdateNoteParameters payload{
        getProgramIndex(), DrumNoteNumber(index + MinDrumNoteNumber)};
    payload.int8_tMemberToUpdate = &performance::NoteParameters::decayMode;
    payload.int8_tValue = std::clamp(i, 0, 1);
    msg.payload = std::move(payload);
    dispatch(std::move(msg));
}

void NoteParameters::setFilterFrequency(const int i) const
{
    performance::PerformanceMessage msg;
    performance::UpdateNoteParameters payload{
        getProgramIndex(), DrumNoteNumber(index + MinDrumNoteNumber)};
    payload.int8_tMemberToUpdate =
        &performance::NoteParameters::filterFrequency;
    payload.int8_tValue = std::clamp(i, 0, 100);
    msg.payload = std::move(payload);
    dispatch(std::move(msg));
}

void NoteParameters::setFilterResonance(const int i) const
{
    performance::PerformanceMessage msg;
    performance::UpdateNoteParameters payload{
        getProgramIndex(), DrumNoteNumber(index + MinDrumNoteNumber)};
    payload.int8_tMemberToUpdate =
        &performance::NoteParameters::filterResonance;
    payload.int8_tValue = std::clamp(i, 0, 15);
    msg.payload = std::move(payload);
    dispatch(std::move(msg));
}

void NoteParameters::setFilterAttack(const int i) const
{
    performance::PerformanceMessage msg;
    performance::UpdateNoteParameters payload{
        getProgramIndex(), DrumNoteNumber(index + MinDrumNoteNumber)};
    payload.int8_tMemberToUpdate = &performance::NoteParameters::filterAttack;
    payload.int8_tValue = std::clamp(i, 0, 100);
    msg.payload = std::move(payload);
    dispatch(std::move(msg));
}

void NoteParameters::setFilterDecay(const int i) const
{
    performance::PerformanceMessage msg;
    performance::UpdateNoteParameters payload{
        getProgramIndex(), DrumNoteNumber(index + MinDrumNoteNumber)};
    payload.int8_tMemberToUpdate = &performance::NoteParameters::filterDecay;
    payload.int8_tValue = std::clamp(i, 0, 100);
    msg.payload = std::move(payload);
    dispatch(std::move(msg));
}

void NoteParameters::setFilterEnvelopeAmount(const int i) const
{
    performance::PerformanceMessage msg;
    performance::UpdateNoteParameters payload{
        getProgramIndex(), DrumNoteNumber(index + MinDrumNoteNumber)};
    payload.int8_tMemberToUpdate =
        &performance::NoteParameters::filterEnvelopeAmount;
    payload.int8_tValue = std::clamp(i, 0, 100);
    msg.payload = std::move(payload);
    dispatch(std::move(msg));
}

void NoteParameters::setVeloToLevel(const int i) const
{
    performance::PerformanceMessage msg;
    performance::UpdateNoteParameters payload{
        getProgramIndex(), DrumNoteNumber(index + MinDrumNoteNumber)};
    payload.int8_tMemberToUpdate =
        &performance::NoteParameters::velocityToLevel;
    payload.int8_tValue = std::clamp(i, 0, 100);
    msg.payload = std::move(payload);
    dispatch(std::move(msg));
}

void NoteParameters::setVelocityToAttack(const int i) const
{
    performance::PerformanceMessage msg;
    performance::UpdateNoteParameters payload{
        getProgramIndex(), DrumNoteNumber(index + MinDrumNoteNumber)};
    payload.int8_tMemberToUpdate =
        &performance::NoteParameters::velocityToAttack;
    payload.int8_tValue = std::clamp(i, 0, 100);
    msg.payload = std::move(payload);
    dispatch(std::move(msg));
}

void NoteParameters::setVelocityToStart(const int i) const
{
    performance::PerformanceMessage msg;
    performance::UpdateNoteParameters payload{
        getProgramIndex(), DrumNoteNumber(index + MinDrumNoteNumber)};
    payload.int8_tMemberToUpdate =
        &performance::NoteParameters::velocityToStart;
    payload.int8_tValue = std::clamp(i, 0, 100);
    msg.payload = std::move(payload);
    dispatch(std::move(msg));
}

void NoteParameters::setVelocityToFilterFrequency(const int i) const
{
    performance::PerformanceMessage msg;
    performance::UpdateNoteParameters payload{
        getProgramIndex(), DrumNoteNumber(index + MinDrumNoteNumber)};
    payload.int8_tMemberToUpdate =
        &performance::NoteParameters::velocityToFilterFrequency;
    payload.int8_tValue = std::clamp(i, 0, 100);
    msg.payload = std::move(payload);
    dispatch(std::move(msg));
}

void NoteParameters::setSliderParameterNumber(const int i) const
{
    performance::PerformanceMessage msg;
    performance::UpdateNoteParameters payload{
        getProgramIndex(), DrumNoteNumber(index + MinDrumNoteNumber)};
    payload.int8_tMemberToUpdate =
        &performance::NoteParameters::sliderParameterNumber;
    payload.int8_tValue = std::clamp(i, 0, 3);
    msg.payload = std::move(payload);
    dispatch(std::move(msg));
}

void NoteParameters::setVelocityToPitch(const int i) const
{
    performance::PerformanceMessage msg;
    performance::UpdateNoteParameters payload{
        getProgramIndex(), DrumNoteNumber(index + MinDrumNoteNumber)};
    payload.int8_tMemberToUpdate =
        &performance::NoteParameters::velocityToPitch;
    payload.int8_tValue = std::clamp(i, -120, 120);
    msg.payload = std::move(payload);
    dispatch(std::move(msg));
}

void NoteParameters::setVoiceOverlapMode(const VoiceOverlapMode m) const
{
    if (m == VoiceOverlapMode::MONO || m == VoiceOverlapMode::NOTE_OFF ||
        m == VoiceOverlapMode::POLY)
    {
        performance::PerformanceMessage msg;
        performance::UpdateNoteParameters payload{
            getProgramIndex(), DrumNoteNumber(index + MinDrumNoteNumber)};
        payload.voiceOverlapModeMemberToUpdate =
            &performance::NoteParameters::voiceOverlapMode;
        payload.voiceOverlapMode = m;
        msg.payload = std::move(payload);
        dispatch(std::move(msg));
    }
}
