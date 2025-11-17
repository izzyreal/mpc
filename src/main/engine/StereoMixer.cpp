#include "StereoMixer.hpp"

using namespace mpc::engine;

StereoMixer::StereoMixer(
    const std::function<performance::StereoMixer()> &getSnapshot,
    const std::function<void(performance::PerformanceMessage &&)> &dispatch)
    : getSnapshot(getSnapshot), dispatch(dispatch)
{
}

void StereoMixer::setPanning(const DrumMixerPanning panning) const
{
    auto s = getSnapshot();
    s.panning = panning;
    performance::PerformanceMessage msg;
    msg.payload = performance::UpdateStereoMixer{s};
    dispatch(std::move(msg));
}

mpc::DrumMixerPanning StereoMixer::getPanning() const
{
    return getSnapshot().panning;
}

void StereoMixer::setLevel(const DrumMixerLevel level) const
{
    auto s = getSnapshot();
    s.level = level;
    performance::PerformanceMessage msg;
    msg.payload = performance::UpdateStereoMixer{s};
    dispatch(std::move(msg));
}

mpc::DrumMixerLevel StereoMixer::getLevel() const
{
    return getSnapshot().level;
}
