#include "StereoMixer.hpp"

using namespace mpc::engine;

StereoMixer::StereoMixer(
    const std::function<performance::StereoMixer()> &getSnapshot,
    const std::function<DrumBusIndex()> &getDrumIndex,
    const std::function<ProgramIndex()> &getProgramIndex,
    const std::function<DrumNoteNumber()> &getDrumNoteNumber,
    const std::function<void(performance::PerformanceMessage &&)> &dispatch)
    : getSnapshot(getSnapshot), getDrumIndex(getDrumIndex), getProgramIndex(getProgramIndex), getDrumNoteNumber(getDrumNoteNumber), dispatch(dispatch)
{
}

void StereoMixer::setPanning(const DrumMixerPanning panning) const
{
    performance::PerformanceMessage msg;
    performance::UpdateStereoMixer payload{getDrumIndex(), getProgramIndex(),
                                           getDrumNoteNumber()};
    payload.member = &performance::StereoMixer::panning;
    payload.newValue = panning;
    msg.payload = std::move(payload);
    dispatch(std::move(msg));
}

mpc::DrumMixerPanning StereoMixer::getPanning() const
{
    return getSnapshot().panning;
}

void StereoMixer::setLevel(const DrumMixerLevel level) const
{
    performance::PerformanceMessage msg;
    performance::UpdateStereoMixer payload{getDrumIndex(), getProgramIndex(),
                                           getDrumNoteNumber()};
    payload.member = &performance::StereoMixer::level;
    payload.newValue = level;
    msg.payload = std::move(payload);
    dispatch(std::move(msg));
}

mpc::DrumMixerLevel StereoMixer::getLevel() const
{
    return getSnapshot().level;
}
