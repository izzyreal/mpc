#include "sequencer/PlaybackState.hpp"

#include "SeqUtil.hpp"
#include "sequencer/Sequence.hpp"

using namespace mpc::sequencer;

mpc::Tick PlaybackState::getCurrentTick(const Sequence *seq,
                                        const TimeInSamples now) const
{
    const auto currentTimeInSamples = now;
    const TimeInSamples deltaSamples =  currentTimeInSamples - originSampleTime;

    const int deltaTicks = SeqUtil::getTickCountForFrames(
        seq,
        originTicks,
        deltaSamples,
        sampleRate
    );

    int currentTick = originTicks + deltaTicks;

    if (currentTick >= seq->getLastTick())
    {
        currentTick = seq->getLastTick() - 1;
    }

    return currentTick;
}
