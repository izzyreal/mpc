#include "IndivFxMixer.hpp"

using namespace mpc::engine;

IndivFxMixer::IndivFxMixer(
    const std::function<performance::IndivFxMixer()> &getSnapshot,
    const std::function<void(performance::PerformanceMessage &&)> &dispatch)
    : getSnapshot(getSnapshot), dispatch(dispatch)
{
}

void IndivFxMixer::setFollowStereo(const bool b) const
{
    auto s = getSnapshot();
    s.followStereo = b;
    performance::PerformanceMessage msg;
    msg.payload = performance::UpdateIndivFxMixer{s};
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
    auto s = getSnapshot();
    s.individualOutput = output;
    performance::PerformanceMessage msg;
    msg.payload = performance::UpdateIndivFxMixer{s};
    dispatch(std::move(msg));
}

void IndivFxMixer::setVolumeIndividualOut(const DrumMixerLevel level) const
{
    auto s = getSnapshot();
    s.individualOutLevel = level;
    performance::PerformanceMessage msg;
    msg.payload = performance::UpdateIndivFxMixer{s};
    dispatch(std::move(msg));
}

mpc::DrumMixerLevel IndivFxMixer::getVolumeIndividualOut() const
{
    return getSnapshot().individualOutLevel;
}

void IndivFxMixer::setFxPath(const DrumMixerIndividualFxPath path) const
{
    auto s = getSnapshot();
    s.fxPath = path;
    performance::PerformanceMessage msg;
    msg.payload = performance::UpdateIndivFxMixer{s};
    dispatch(std::move(msg));
}

mpc::DrumMixerIndividualFxPath IndivFxMixer::getFxPath() const
{
    return getSnapshot().fxPath;
}

void IndivFxMixer::setFxSendLevel(const DrumMixerLevel level) const
{
    auto s = getSnapshot();
    s.fxSendLevel = level;
    performance::PerformanceMessage msg;
    msg.payload = performance::UpdateIndivFxMixer{s};
    dispatch(std::move(msg));
}

mpc::DrumMixerLevel IndivFxMixer::getFxSendLevel() const
{
    return getSnapshot().fxSendLevel;
}
