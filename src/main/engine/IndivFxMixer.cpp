#include "IndivFxMixer.hpp"

using namespace mpc::engine;

IndivFxMixer::IndivFxMixer(
    const std::function<performance::IndivFxMixer()> &getSnapshot,
    const std::function<DrumBusIndex()> &getDrumIndex,
    const std::function<ProgramIndex()> &getProgramIndex,
    const std::function<DrumNoteNumber()> &getDrumNoteNumber,
    const std::function<void(performance::PerformanceMessage &&)> &dispatch)
    : getSnapshot(getSnapshot), getDrumIndex(getDrumIndex),
      getProgramIndex(getProgramIndex), getDrumNoteNumber(getDrumNoteNumber),
      dispatch(dispatch)
{
}

void IndivFxMixer::setFollowStereo(const bool b) const
{
    performance::UpdateIndivFxMixer msg{getDrumIndex(), getProgramIndex(),
                                        getDrumNoteNumber()};
    msg.followStereoMember = &performance::IndivFxMixer::followStereo;
    msg.followStereo = b;
    dispatch(std::move(msg));
}

bool IndivFxMixer::isFollowingStereo() const
{
    return getSnapshot().followStereo;
}

mpc::DrumMixerIndividualOutput IndivFxMixer::getOutput() const
{
    return getSnapshot().individualOutput;
}

void IndivFxMixer::setOutput(const DrumMixerIndividualOutput output) const
{
    performance::UpdateIndivFxMixer msg{getDrumIndex(), getProgramIndex(),
                                        getDrumNoteNumber()};
    msg.individualOutputMember = &performance::IndivFxMixer::individualOutput;
    msg.individualOutput = output;
    dispatch(std::move(msg));
}

void IndivFxMixer::setVolumeIndividualOut(const DrumMixerLevel level) const
{
    performance::UpdateIndivFxMixer msg{getDrumIndex(), getProgramIndex(),
                                        getDrumNoteNumber()};
    msg.value0To100Member = &performance::IndivFxMixer::individualOutLevel;
    msg.newValue = level;
    dispatch(std::move(msg));
}

mpc::DrumMixerLevel IndivFxMixer::getVolumeIndividualOut() const
{
    return getSnapshot().individualOutLevel;
}

void IndivFxMixer::setFxPath(const DrumMixerIndividualFxPath path) const
{
    performance::UpdateIndivFxMixer msg{getDrumIndex(), getProgramIndex(),
                                        getDrumNoteNumber()};
    msg.individualFxPathMember = &performance::IndivFxMixer::fxPath;
    msg.individualFxPath = path;
    dispatch(std::move(msg));
}

mpc::DrumMixerIndividualFxPath IndivFxMixer::getFxPath() const
{
    return getSnapshot().fxPath;
}

void IndivFxMixer::setFxSendLevel(const DrumMixerLevel level) const
{
    performance::UpdateIndivFxMixer msg{getDrumIndex(), getProgramIndex(),
                                        getDrumNoteNumber()};
    msg.value0To100Member = &performance::IndivFxMixer::fxSendLevel;
    msg.newValue = level;
    dispatch(std::move(msg));
}

mpc::DrumMixerLevel IndivFxMixer::getFxSendLevel() const
{
    return getSnapshot().fxSendLevel;
}
