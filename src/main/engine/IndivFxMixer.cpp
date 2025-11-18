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
    performance::PerformanceMessage msg;
    performance::UpdateIndivFxMixer payload{getDrumIndex(), getProgramIndex(),
                                            getDrumNoteNumber()};
    payload.followStereoMember = &performance::IndivFxMixer::followStereo;
    payload.followStereo = b;
    msg.payload = std::move(payload);
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
    performance::PerformanceMessage msg;
    performance::UpdateIndivFxMixer payload{getDrumIndex(), getProgramIndex(),
                                            getDrumNoteNumber()};
    payload.individualOutputMember =
        &performance::IndivFxMixer::individualOutput;
    payload.individualOutput = output;
    msg.payload = std::move(payload);
    dispatch(std::move(msg));
}

void IndivFxMixer::setVolumeIndividualOut(const DrumMixerLevel level) const
{
    performance::PerformanceMessage msg;
    performance::UpdateIndivFxMixer payload{getDrumIndex(), getProgramIndex(),
                                            getDrumNoteNumber()};
    payload.value0To100Member = &performance::IndivFxMixer::individualOutLevel;
    payload.newValue = level;
    msg.payload = std::move(payload);
    dispatch(std::move(msg));
}

mpc::DrumMixerLevel IndivFxMixer::getVolumeIndividualOut() const
{
    return getSnapshot().individualOutLevel;
}

void IndivFxMixer::setFxPath(const DrumMixerIndividualFxPath path) const
{
    performance::PerformanceMessage msg;
    performance::UpdateIndivFxMixer payload{getDrumIndex(), getProgramIndex(),
                                            getDrumNoteNumber()};
    payload.individualFxPathMember = &performance::IndivFxMixer::fxPath;
    payload.individualFxPath = path;
    msg.payload = std::move(payload);
    dispatch(std::move(msg));
}

mpc::DrumMixerIndividualFxPath IndivFxMixer::getFxPath() const
{
    return getSnapshot().fxPath;
}

void IndivFxMixer::setFxSendLevel(const DrumMixerLevel level) const
{
    performance::PerformanceMessage msg;
    performance::UpdateIndivFxMixer payload{getDrumIndex(), getProgramIndex(),
                                            getDrumNoteNumber()};
    payload.value0To100Member = &performance::IndivFxMixer::fxSendLevel;
    payload.newValue = level;
    msg.payload = std::move(payload);
    dispatch(std::move(msg));
}

mpc::DrumMixerLevel IndivFxMixer::getFxSendLevel() const
{
    return getSnapshot().fxSendLevel;
}
