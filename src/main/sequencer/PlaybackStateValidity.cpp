#include "sequencer/PlaybackStateValidity.hpp"

#include "RenderContext.hpp"
#include "SeqUtil.hpp"
#include "Sequencer.hpp"

constexpr mpc::TimeInSamples safetyMargin = 10000;

void mpc::sequencer::computeSafeValidity(RenderContext &renderCtx,
                                         const TimeInSamples currentTime,
                                         const TimeInSamples strictValidUntil)
{
    const TimeInSamples safeValidUntil = strictValidUntil - safetyMargin;

    const TimeInSamples safeValidFrom = currentTime + safetyMargin;

    const auto framesUntil =
        safeValidUntil - renderCtx.playbackState.strictValidFrom;

    const auto framesFrom =
        safeValidFrom - renderCtx.playbackState.strictValidFrom;

    const Tick baseTick = renderCtx.playbackState.originTicks;

    const Tick tickAdvanceUntil =
        SeqUtil::getTickCountForFrames(renderCtx.seq, baseTick, framesUntil,
                                       renderCtx.playbackState.sampleRate);
    const Tick tickAdvanceFrom =
        SeqUtil::getTickCountForFrames(renderCtx.seq, baseTick, framesFrom,
                                       renderCtx.playbackState.sampleRate);

    const Tick untilTick = baseTick + tickAdvanceUntil;
    const Tick fromTick = baseTick + tickAdvanceFrom;

    renderCtx.playbackState.safeValidUntil = safeValidUntil;
    renderCtx.playbackState.safeValidUntilTick = untilTick;
    renderCtx.playbackState.safeValidUntilQuarterNote =
        Sequencer::ticksToQuarterNotes(untilTick);

    renderCtx.playbackState.safeValidFrom = safeValidFrom;
    renderCtx.playbackState.safeValidFromTick = fromTick;
    renderCtx.playbackState.safeValidFromQuarterNote =
        Sequencer::ticksToQuarterNotes(fromTick);
}
