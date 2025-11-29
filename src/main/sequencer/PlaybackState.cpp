#include "sequencer/PlaybackState.hpp"

#include "SeqUtil.hpp"
#include "Sequencer.hpp"
#include "sequencer/Sequence.hpp"

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
    const auto currentTimeInSamples = now;
    const TimeInSamples deltaSamples =  currentTimeInSamples - lastTransitionTime;

    const double deltaTicks = SeqUtil::getTickCountForFrames(
        seq,
        lastTransitionTick,
        deltaSamples,
        sampleRate
    );

    const double currentTick = lastTransitionTick + deltaTicks;

    return currentTick;
}
