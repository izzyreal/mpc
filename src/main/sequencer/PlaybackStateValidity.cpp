#include "sequencer/PlaybackStateValidity.hpp"

#include "RenderContext.hpp"
#include "SeqUtil.hpp"

constexpr mpc::TimeInSamples safetyMargin = 5000;

void mpc::sequencer::computeValidity(RenderContext &renderCtx,
                                     const TimeInSamples currentTime)
{
    constexpr float renderBlockSizeSeconds = 1.0;

    auto &state = renderCtx.playbackState;
    const auto seq = renderCtx.seq;
    const auto sampleRate = state.sampleRate;

    const TimeInSamples renderBlockSizeSamples{sampleRate * renderBlockSizeSeconds};

    state.strictValidFrom = currentTime;
    state.strictValidUntil = currentTime + renderBlockSizeSamples;

    const TimeInSamples sinceTransition = currentTime - state.lastTransitionTime;

    state.strictValidFromTick = SeqUtil::getTickCountForFrames(
        seq, state.lastTransitionTick, sinceTransition, sampleRate);

    const double ticksInBlock = SeqUtil::getTickCountForFrames(
        seq, state.strictValidFromTick, renderBlockSizeSamples, sampleRate);

    state.strictValidUntilTick = state.strictValidFromTick + ticksInBlock;

    state.safeValidFrom = state.strictValidFrom + safetyMargin;
    state.safeValidUntil = state.strictValidUntil - safetyMargin;

    const TimeInSamples safeFromDelta = state.safeValidFrom - state.strictValidFrom;
    const TimeInSamples safeUntilDelta = state.strictValidUntil - state.safeValidUntil;

    state.safeValidFromTick =
        state.strictValidFromTick +
        SeqUtil::getTickCountForFrames(
            seq, state.strictValidFromTick, safeFromDelta, sampleRate);

    state.safeValidUntilTick =
        state.strictValidUntilTick -
        SeqUtil::getTickCountForFrames(
            seq, state.safeValidUntilTick, safeUntilDelta, sampleRate);
}
