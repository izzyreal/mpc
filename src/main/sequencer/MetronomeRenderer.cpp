#include "sequencer/MetronomeRenderer.hpp"

#include "SequenceStateView.hpp"
#include "sequencer/RenderContext.hpp"

#include "lcdgui/screens/window/CountMetronomeScreen.hpp"
#include "utils/SequencerTiming.hpp"

mpc::sequencer::MetronomeRenderContext
mpc::sequencer::initMetronomeRenderContext(
    const std::function<bool(std::initializer_list<lcdgui::ScreenId>)>
        &isCurrentScreen,
    const std::function<bool()> &isRecMainWithoutPlaying,
    const std::function<std::shared_ptr<lcdgui::Screens>()> &getScreens)
{
    const bool isStepEditor =
        isCurrentScreen({lcdgui::ScreenId::StepEditorScreen});

    const auto countMetronomeScreen =
        getScreens()->get<lcdgui::ScreenId::CountMetronomeScreen>().get();

    return {countMetronomeScreen, isStepEditor, isRecMainWithoutPlaying()};
}

void mpc::sequencer::renderMetronome(RenderContext &ctx,
                                     const MetronomeRenderContext &mctx)
{
    if (!ctx.transportStateView.isCountEnabled())
    {
        return;
    }

    if (ctx.transportStateView.isRecordingOrOverdubbing())
    {
        if (!mctx.countMetronomeScreen->getInRec() &&
            !ctx.transportStateView.isCountingIn())
        {
            return;
        }
    }
    else
    {
        if (!mctx.isStepEditor && !mctx.countMetronomeScreen->getInPlay() &&
            !ctx.transportStateView.isCountingIn() &&
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

        auto seqTimingData = utils::getSequenceTimingData(*ctx.seq);

        for (int j = 0; j < barLength; j += denTicks)
        {
            const auto eventTick = j + barTickOffset;
            const auto eventTickToUse =
                eventTick - ctx.playbackState.lastTransitionTick;

            const auto eventTime = utils::getEventTimeInSamples(
                *ctx.seq, eventTickToUse, ctx.playbackState.strictValidFrom,
                ctx.playbackState.sampleRate);

            if (!ctx.playbackState.containsTimeInSamplesStrict(eventTime))
            {
                continue;
            }

            EventState eventState;
            eventState.tick = j;
            eventState.type = EventType::MetronomeClick;
            eventState.velocity = j == 0 ? MaxVelocity : MediumVelocity;
            ctx.playbackState.events.emplace_back(
                RenderedEventState{std::move(eventState), eventTime});
        }

        barTickOffset += barLength;
    }
}
