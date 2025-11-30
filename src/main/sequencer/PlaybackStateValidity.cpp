#include "sequencer/PlaybackStateValidity.hpp"

#include "RenderContext.hpp"
#include "TempoChangeEvent.hpp"
#include "utils/SequencerTiming.hpp"

void mpc::sequencer::computeValidity(RenderContext &renderCtx,
                                     const TimeInSamples currentTime)
{
    constexpr int bufferSize = 512;
    auto &state = renderCtx.playbackState;
    const auto seq = renderCtx.seq;
    const auto sampleRate = state.sampleRate;

    constexpr TimeInSamples renderBlockSizeSamples{bufferSize * 2};

    state.strictValidFrom = currentTime;
    state.strictValidUntil = currentTime + renderBlockSizeSamples;

    const TimeInSamples sinceTransition =
        currentTime - state.lastTransitionTime;

    const auto sequenceTimingData = utils::getSequenceTimingData(*seq);

    state.strictValidFromTick = utils::getTickCountForFrames(
        sequenceTimingData, state.lastTransitionTick, sinceTransition, sampleRate);

    const double ticksInBlock = utils::getTickCountForFrames(
        sequenceTimingData, state.strictValidFromTick, renderBlockSizeSamples, sampleRate);

    state.strictValidUntilTick = state.strictValidFromTick + ticksInBlock;
}
