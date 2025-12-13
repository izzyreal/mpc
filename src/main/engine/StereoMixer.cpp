#include "StereoMixer.hpp"

using namespace mpc::engine;

StereoMixer::StereoMixer(
    const std::function<performance::StereoMixer()> &getSnapshot,
    const std::function<DrumBusIndex()> &getDrumIndex,
    const std::function<ProgramIndex()> &getProgramIndex,
    const std::function<DrumNoteNumber()> &getDrumNoteNumber,
    const std::function<void(performance::PerformanceMessage &&)> &dispatch)
    : getSnapshot(getSnapshot), getDrumIndex(getDrumIndex),
      getProgramIndex(getProgramIndex), getDrumNoteNumber(getDrumNoteNumber),
      dispatch(dispatch)
{
}

void StereoMixer::setPanning(const DrumMixerPanning panning) const
{
    performance::UpdateStereoMixer msg{getDrumIndex(), getProgramIndex(),
                                       getDrumNoteNumber()};
    msg.member = &performance::StereoMixer::panning;
    msg.newValue = panning;
    dispatch(std::move(msg));
}

mpc::DrumMixerPanning StereoMixer::getPanning() const
{
    return getSnapshot().panning;
}

void StereoMixer::setLevel(const DrumMixerLevel level) const
{
    performance::UpdateStereoMixer msg{getDrumIndex(), getProgramIndex(),
                                       getDrumNoteNumber()};
    msg.member = &performance::StereoMixer::level;
    msg.newValue = level;
    dispatch(std::move(msg));
}

mpc::DrumMixerLevel StereoMixer::getLevel() const
{
    return getSnapshot().level;
}
