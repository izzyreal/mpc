#include "sequencer/PlaybackState.hpp"

#include "Sequencer.hpp"
#include "sequencer/Sequence.hpp"
#include "utils/SequencerTiming.hpp"

using namespace mpc::sequencer;

mpc::PositionQuarterNotes PlaybackState::getSafeValidFromQN() const
{
    return Sequencer::ticksToQuarterNotes(safeValidFromTick);
}

mpc::PositionQuarterNotes PlaybackState::getSafeValidUntilQN() const
{
    return Sequencer::ticksToQuarterNotes(safeValidUntilTick);
}

mpc::PositionQuarterNotes PlaybackState::getLastTransitionQN() const
{
    return Sequencer::ticksToQuarterNotes(lastTransitionTick);
}

mpc::PositionQuarterNotes PlaybackState::getStrictValidFromQN() const
{
    return Sequencer::ticksToQuarterNotes(strictValidFromTick);
}

mpc::PositionQuarterNotes PlaybackState::getStrictValidUntilQN() const
{
    return Sequencer::ticksToQuarterNotes(strictValidUntilTick);
}

double PlaybackState::getCurrentTick(const Sequence *seq,
                                     const TimeInSamples now) const
{
    const TimeInSamples deltaSamples = now - lastTransitionTime;

    const auto sequenceTimingData = utils::getSequenceTimingData(seq);

    const double deltaTicks = utils::getTickCountForFrames(
        sequenceTimingData, lastTransitionTick, deltaSamples, sampleRate);

    const double currentTick = fmod(lastTransitionTick + deltaTicks,
                                    static_cast<double>(seq->getLastTick()));

    // printf("now: %lld, lastTransitionTime: %lld, deltaSamples: %lld,
    // currenTick: %f\n", now, lastTransitionTime, deltaSamples, currentTick);

    return currentTick;
}
