#include "sampler/NoteParameters.hpp"

#include "IntTypes.hpp"

#include "engine/IndivFxMixer.hpp"
#include "engine/StereoMixer.hpp"

using namespace mpc::sampler;

NoteParameters::NoteParameters(
    const int index,
    const std::function<performance::NoteParameters()> &getSnapshot,
    const std::function<void(performance::PerformanceMessage &)> &dispatch)
    : index(index), getSnapshot(getSnapshot), dispatch(dispatch)
{
    stereoMixer = std::make_shared<engine::StereoMixer>(
        [&]
        {
            return getSnapshot().stereoMixer;
        },
        dispatch);
    indivFxMixer = std::make_shared<engine::IndivFxMixer>(
        [&]
        {
            return getSnapshot().indivFxMixer;
        },
        dispatch);
}

NoteParameters *NoteParameters::clone(const int newIndex) const
{
    const auto res = new NoteParameters(newIndex, getSnapshot, dispatch);
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

void NoteParameters::setSoundIndex(const int i) const
{
    auto s = getSnapshot();
    s.soundIndex = std::clamp(i, -1, 999999);
    performance::PerformanceMessage msg;
    msg.payload = performance::UpdateNoteParameters{{}, {}, s};
    dispatch(msg);
}

void NoteParameters::setSoundGenMode(const int i) const
{
    auto s = getSnapshot();
    s.soundGenerationMode = std::clamp(i, 0, 3);
    performance::PerformanceMessage msg;
    msg.payload = performance::UpdateNoteParameters{{}, {}, s};
    dispatch(msg);
}

int NoteParameters::getSoundGenerationMode() const
{
    return getSnapshot().soundGenerationMode;
}

void NoteParameters::setVeloRangeLower(const int i) const
{
    auto s = getSnapshot();
    s.velocityRangeLower = std::clamp(i, 0, 126);
    if (s.velocityRangeUpper <= s.velocityRangeLower)
    {
        s.velocityRangeUpper = s.velocityRangeLower + 1;
    }
    performance::PerformanceMessage msg;
    msg.payload = performance::UpdateNoteParameters{{}, {}, s};
    dispatch(msg);
}

void NoteParameters::setVeloRangeUpper(const int i) const
{
    auto s = getSnapshot();
    s.velocityRangeUpper = std::clamp(i, 1, 127);
    if (s.velocityRangeLower >= s.velocityRangeUpper)
    {
        s.velocityRangeLower = s.velocityRangeUpper - 1;
    }
    performance::PerformanceMessage msg;
    msg.payload = performance::UpdateNoteParameters{{}, {}, s};
    dispatch(msg);
}

int NoteParameters::getVelocityRangeLower() const
{
    return getSnapshot().velocityRangeLower;
}

int NoteParameters::getVelocityRangeUpper() const
{
    return getSnapshot().velocityRangeUpper;
}

void NoteParameters::setOptNoteA(const DrumNoteNumber i) const
{
    auto s = getSnapshot();
    s.optionalNoteA = std::clamp(i, NoDrumNoteAssigned, MaxDrumNoteNumber);
    performance::PerformanceMessage msg;
    msg.payload = performance::UpdateNoteParameters{{}, {}, s};
    dispatch(msg);
}

mpc::DrumNoteNumber NoteParameters::getOptionalNoteA() const
{
    return getSnapshot().optionalNoteA;
}

void NoteParameters::setOptionalNoteB(const DrumNoteNumber i) const
{
    auto s = getSnapshot();
    s.optionalNoteB = std::clamp(i, NoDrumNoteAssigned, MaxDrumNoteNumber);
    performance::PerformanceMessage msg;
    msg.payload = performance::UpdateNoteParameters{{}, {}, s};
    dispatch(msg);
}

mpc::DrumNoteNumber NoteParameters::getOptionalNoteB() const
{
    return getSnapshot().optionalNoteB;
}

VoiceOverlapMode NoteParameters::getVoiceOverlapMode() const
{
    return getSnapshot().voiceOverlapMode;
}

void NoteParameters::setVoiceOverlapMode(const VoiceOverlapMode m) const
{
    auto s = getSnapshot();
    if (m == VoiceOverlapMode::MONO || m == VoiceOverlapMode::NOTE_OFF ||
        m == VoiceOverlapMode::POLY)
    {
        s.voiceOverlapMode = m;
        performance::PerformanceMessage msg;
        msg.payload = performance::UpdateNoteParameters{{}, {}, s};
        dispatch(msg);
    }
}

void NoteParameters::setMuteAssignA(const DrumNoteNumber i) const
{
    auto s = getSnapshot();
    s.muteAssignA = std::clamp(i, NoDrumNoteAssigned, MaxDrumNoteNumber);
    performance::PerformanceMessage msg;
    msg.payload = performance::UpdateNoteParameters{{}, {}, s};
    dispatch(msg);
}

mpc::DrumNoteNumber NoteParameters::getMuteAssignA() const
{
    return getSnapshot().muteAssignA;
}

void NoteParameters::setMuteAssignB(const DrumNoteNumber i) const
{
    auto s = getSnapshot();
    s.muteAssignB = std::clamp(i, NoDrumNoteAssigned, MaxDrumNoteNumber);
    performance::PerformanceMessage msg;
    msg.payload = performance::UpdateNoteParameters{{}, {}, s};
    dispatch(msg);
}

mpc::DrumNoteNumber NoteParameters::getMuteAssignB() const
{
    return getSnapshot().muteAssignB;
}

void NoteParameters::setTune(const int i) const
{
    auto s = getSnapshot();
    s.tune = std::clamp(i, -240, 240);
    performance::PerformanceMessage msg;
    msg.payload = performance::UpdateNoteParameters{{}, {}, s};
    dispatch(msg);
}

int NoteParameters::getTune() const
{
    return getSnapshot().tune;
}

void NoteParameters::setAttack(const int i) const
{
    auto s = getSnapshot();
    s.attack = std::clamp(i, 0, 100);
    performance::PerformanceMessage msg;
    msg.payload = performance::UpdateNoteParameters{{}, {}, s};
    dispatch(msg);
}

int NoteParameters::getAttack() const
{
    return getSnapshot().attack;
}

void NoteParameters::setDecay(const int i) const
{
    auto s = getSnapshot();
    s.decay = std::clamp(i, 0, 100);
    performance::PerformanceMessage msg;
    msg.payload = performance::UpdateNoteParameters{{}, {}, s};
    dispatch(msg);
}

int NoteParameters::getDecay() const
{
    return getSnapshot().decay;
}

void NoteParameters::setDecayMode(const int i) const
{
    auto s = getSnapshot();
    s.decayMode = std::clamp(i, 0, 1);
    performance::PerformanceMessage msg;
    msg.payload = performance::UpdateNoteParameters{{}, {}, s};
    dispatch(msg);
}

int NoteParameters::getDecayMode() const
{
    return getSnapshot().decayMode;
}

void NoteParameters::setFilterFrequency(const int i) const
{
    auto s = getSnapshot();
    s.filterFrequency = std::clamp(i, 0, 100);
    performance::PerformanceMessage msg;
    msg.payload = performance::UpdateNoteParameters{{}, {}, s};
    dispatch(msg);
}

int NoteParameters::getFilterFrequency() const
{
    return getSnapshot().filterFrequency;
}

void NoteParameters::setFilterResonance(const int i) const
{
    auto s = getSnapshot();
    s.filterResonance = std::clamp(i, 0, 15);
    performance::PerformanceMessage msg;
    msg.payload = performance::UpdateNoteParameters{{}, {}, s};
    dispatch(msg);
}

int NoteParameters::getFilterResonance() const
{
    return getSnapshot().filterResonance;
}

void NoteParameters::setFilterAttack(const int i) const
{
    auto s = getSnapshot();
    s.filterAttack = std::clamp(i, 0, 100);
    performance::PerformanceMessage msg;
    msg.payload = performance::UpdateNoteParameters{{}, {}, s};
    dispatch(msg);
}

int NoteParameters::getFilterAttack() const
{
    return getSnapshot().filterAttack;
}

void NoteParameters::setFilterDecay(const int i) const
{
    auto s = getSnapshot();
    s.filterDecay = std::clamp(i, 0, 100);
    performance::PerformanceMessage msg;
    msg.payload = performance::UpdateNoteParameters{{}, {}, s};
    dispatch(msg);
}

int NoteParameters::getFilterDecay() const
{
    return getSnapshot().filterDecay;
}

void NoteParameters::setFilterEnvelopeAmount(const int i) const
{
    auto s = getSnapshot();
    s.filterEnvelopeAmount = std::clamp(i, 0, 100);
    performance::PerformanceMessage msg;
    msg.payload = performance::UpdateNoteParameters{{}, {}, s};
    dispatch(msg);
}

int NoteParameters::getFilterEnvelopeAmount() const
{
    return getSnapshot().filterEnvelopeAmount;
}

void NoteParameters::setVeloToLevel(const int i) const
{
    auto s = getSnapshot();
    s.velocityToLevel = std::clamp(i, 0, 100);
    performance::PerformanceMessage msg;
    msg.payload = performance::UpdateNoteParameters{{}, {}, s};
    dispatch(msg);
}

int NoteParameters::getVeloToLevel() const
{
    return getSnapshot().velocityToLevel;
}

void NoteParameters::setVelocityToAttack(const int i) const
{
    auto s = getSnapshot();
    s.velocityToAttack = std::clamp(i, 0, 100);
    performance::PerformanceMessage msg;
    msg.payload = performance::UpdateNoteParameters{{}, {}, s};
    dispatch(msg);
}

int NoteParameters::getVelocityToAttack() const
{
    return getSnapshot().velocityToAttack;
}

void NoteParameters::setVelocityToStart(const int i) const
{
    auto s = getSnapshot();
    s.velocityToStart = std::clamp(i, 0, 100);
    performance::PerformanceMessage msg;
    msg.payload = performance::UpdateNoteParameters{{}, {}, s};
    dispatch(msg);
}

int NoteParameters::getVelocityToStart() const
{
    return getSnapshot().velocityToStart;
}

void NoteParameters::setVelocityToFilterFrequency(const int i) const
{
    auto s = getSnapshot();
    s.velocityToFilterFrequency = std::clamp(i, 0, 100);
    performance::PerformanceMessage msg;
    msg.payload = performance::UpdateNoteParameters{{}, {}, s};
    dispatch(msg);
}

int NoteParameters::getVelocityToFilterFrequency() const
{
    return getSnapshot().velocityToFilterFrequency;
}

void NoteParameters::setSliderParameterNumber(const int i) const
{
    auto s = getSnapshot();
    s.sliderParameterNumber = std::clamp(i, 0, 3);
    performance::PerformanceMessage msg;
    msg.payload = performance::UpdateNoteParameters{{}, {}, s};
    dispatch(msg);
}

int NoteParameters::getSliderParameterNumber() const
{
    return getSnapshot().sliderParameterNumber;
}

void NoteParameters::setVelocityToPitch(const int i) const
{
    auto s = getSnapshot();
    s.velocityToPitch = std::clamp(i, -120, 120);
    performance::PerformanceMessage msg;
    msg.payload = performance::UpdateNoteParameters{{}, {}, s};
    dispatch(msg);
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
