#include "sequencer/MetronomeRenderer.hpp"

#include "SeqUtil.hpp"
#include "Sequence.hpp"
#include "Sequencer.hpp"
#include "Transport.hpp"
#include "sequencer/RenderContext.hpp"

#include "lcdgui/screens/window/CountMetronomeScreen.hpp"

void mpc::sequencer::renderMetronome(RenderContext &ctx, const MetronomeRenderContext &mctx)
{
    if (!ctx.sequencer->getTransport()->isCountEnabled())
    {
        return;
    }

    if (ctx.sequencer->getTransport()->isRecordingOrOverdubbing())
    {
        if (!mctx.countMetronomeScreen->getInRec() &&
            !ctx.sequencer->getTransport()->isCountingIn())
        {
            return;
        }
    }
    else
    {
        if (!mctx.isStepEditor && !mctx.countMetronomeScreen->getInPlay() &&
            !ctx.sequencer->getTransport()->isCountingIn() &&
            !mctx.isRecMainWithoutPlaying)
        {
            return;
        }
    }

    const auto metronomeRate = mctx.countMetronomeScreen->getRate();

    int barTickOffset = 0;

    for (int i = 0; i < ctx.seq->getBarCount(); ++i)
    {
        const auto ts = ctx.seq->getTimeSignature(i);
        const auto den = ts.denominator;
        auto denTicks = 96 * (4.0 / den);

        switch (metronomeRate)
        {
            case 1:
                denTicks *= 2.0f / 3.f;
                break;
            case 2:
                denTicks *= 1.0f / 2;
                break;
            case 3:
                denTicks *= 1.0f / 3;
                break;
            case 4:
                denTicks *= 1.0f / 4;
                break;
            case 5:
                denTicks *= 1.0f / 6;
                break;
            case 6:
                denTicks *= 1.0f / 8;
                break;
            case 7:
                denTicks *= 1.0f / 12;
                break;
            default:;
        }

        const auto barLength = ts.getBarLength();

        for (int j = 0; j < barLength; j += denTicks)
        {
            const auto eventTick = j + barTickOffset;
            const auto eventTickToUse =
                eventTick - ctx.playbackState.originTicks;

            const auto eventTimeInSamples = SeqUtil::getEventTimeInSamples(
                ctx.seq, eventTickToUse,
                ctx.playbackState.strictValidFrom,
                ctx.playbackState.sampleRate);

            if (!ctx.playbackState.containsTimeInSamplesStrict(
                    eventTimeInSamples))
            {
                continue;
            }

            EventState eventState;
            eventState.tick = j;
            eventState.type = EventType::MetronomeClick;
            eventState.velocity =
                j == 0 ? MaxVelocity : MediumVelocity;
            ctx.playbackState.events.emplace_back(
                RenderedEventState{std::move(eventState), eventTimeInSamples});
        }

        barTickOffset += barLength;
    }
}
