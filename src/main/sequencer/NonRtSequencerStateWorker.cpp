#include "sequencer/NonRtSequencerStateWorker.hpp"

#include "Event.hpp"
#include "SeqUtil.hpp"
#include "Sequence.hpp"
#include "Track.hpp"
#include "NonRtSequencerStateManager.hpp"
#include "SequencerStateManager.hpp"
#include "TimeSignature.hpp"
#include "Transport.hpp"
#include "engine/SequencerPlaybackEngine.hpp"
#include "lcdgui/ScreenComponent.hpp"
#include "lcdgui/Screens.hpp"
#include "lcdgui/screens/window/CountMetronomeScreen.hpp"
#include "sequencer/Sequencer.hpp"

using namespace mpc::sequencer;

NonRtSequencerStateWorker::NonRtSequencerStateWorker(
    const std::function<bool(std::initializer_list<lcdgui::ScreenId>)>
        &isCurrentScreen,
    const std::function<bool()> &isRecMainWithoutPlaying, Sequencer *sequencer)
    : running(false), isCurrentScreen(isCurrentScreen),
      isRecMainWithoutPlaying(isRecMainWithoutPlaying), sequencer(sequencer)
{
}

NonRtSequencerStateWorker::~NonRtSequencerStateWorker()
{
    stop();

    if (workerThread.joinable())
    {
        workerThread.join();
    }
}

void NonRtSequencerStateWorker::start()
{
    if (running.load())
    {
        return;
    }

    running.store(true);

    if (workerThread.joinable())
    {
        workerThread.join();
    }

    workerThread = std::thread(
        [&]
        {
            while (running.load())
            {
                work();
                std::this_thread::sleep_for(std::chrono::milliseconds(3));
            }
        });
}

void NonRtSequencerStateWorker::stop()
{
    if (!running.load())
    {
        return;
    }

    running.store(false);
}

void NonRtSequencerStateWorker::stopAndWaitUntilStopped()
{
    stop();

    if (workerThread.joinable())
    {
        workerThread.join();
    }
}

void NonRtSequencerStateWorker::work() const
{
    const auto snapshot = sequencer->getNonRtStateManager()->getSnapshot();
    const auto playbackState = snapshot.getPlaybackState();

    const auto currentTimeInSamples =
        sequencer->getStateManager()->getSnapshot().getTimeInSamples();

    bool snapshotIsInvalid = false;

    if (snapshot.getPositionQuarterNotes() != NoPositionQuarterNotes &&
        currentTimeInSamples >
            snapshot.getPlaybackState().strictValidUntilTimeInSamples -
                playbackStateValiditySafetyMarginTimeInSamples)
    {
        snapshotIsInvalid = true;
    }

    if (snapshotIsInvalid)
    {
        constexpr auto onComplete = [] {};
        refreshPlaybackState(snapshot.getPlaybackState().playOffsetQuarterNotes,
                             currentTimeInSamples, onComplete);
    }

    sequencer->getNonRtStateManager()->drainQueue();
}

void NonRtSequencerStateWorker::refreshPlaybackState(
    const PositionQuarterNotes playOffset, const TimeInSamples timeInSamples,
    const std::function<void()> &onComplete) const
{
    TimeInSamples timeInSamplesToUse = timeInSamples;

    if (timeInSamplesToUse == CurrentTimeInSamples)
    {
        timeInSamplesToUse =
            sequencer->getStateManager()->getSnapshot().getTimeInSamples();
        printf("Time in samples to use: %i\n", timeInSamplesToUse);
    }

    const auto playbackEngine = sequencer->getSequencerPlaybackEngine();
    const auto sampleRate = playbackEngine->getSampleRate();

    const auto playbackState = renderPlaybackState(
        SampleRate(sampleRate), playOffset, timeInSamplesToUse);

    sequencer->getNonRtStateManager()->enqueue(
        UpdatePlaybackState{std::move(playbackState), onComplete});
}

Sequencer *NonRtSequencerStateWorker::getSequencer() const
{
    return sequencer;
}

struct RenderContext
{
    PlaybackState playbackState;
    Sequencer *sequencer;
    Sequence *seq;
};

struct MetronomeRenderContext
{
    CountMetronomeScreen *countMetronomeScreen;
    bool isStepEditor;
    bool isRecMainWithoutPlaying;
};

void renderMetronome(RenderContext &ctx, const MetronomeRenderContext &mctx)
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
            const auto eventTickToUse =
                j + barTickOffset -
                Sequencer::quarterNotesToTicks(
                    ctx.playbackState.playOffsetQuarterNotes);

            const auto eventTimeInSamples = SeqUtil::getEventTimeInSamples(
                ctx.seq, eventTickToUse, ctx.playbackState.currentTimeInSamples,
                ctx.playbackState.sampleRate);

            if (eventTimeInSamples > ctx.playbackState.strictValidUntilTimeInSamples)
            {
                continue;
            }

            EventState eventState;
            eventState.tick = j;
            eventState.type = EventType::MetronomeClick;
            eventState.velocity =
                j == 0 ? mpc::MaxVelocity : mpc::MediumVelocity;
            ctx.playbackState.events.emplace_back(
                RenderedEventState{std::move(eventState), eventTimeInSamples});
        }

        barTickOffset += barLength;
    }
}

void renderSeq(RenderContext &ctx)
{
    // printf("Rendering seq %s for playback time %lld at tick offset %i\n",
           // ctx.seq->getName().c_str(), ctx.playbackState.currentTimeInSamples,
           // Sequencer::quarterNotesToTicks(
               // ctx.playbackState.playOffsetQuarterNotes));
    for (const auto &track : ctx.seq->getTracks())
    {
        for (const auto &event : track->getEvents())
        {
            const auto eventState = event->getSnapshot();

            // printf("Rendering event with seq %i track %i tick %i\n",
            // eventState.sequenceIndex, eventState.trackIndex,
            // eventState.tick);

            const auto eventTickToUse =
                eventState.tick - Sequencer::quarterNotesToTicks(
                                      ctx.playbackState.playOffsetQuarterNotes);

            const mpc::TimeInSamples eventTime = SeqUtil::getEventTimeInSamples(
                ctx.seq, eventTickToUse, ctx.playbackState.currentTimeInSamples,
                ctx.playbackState.sampleRate);

            if (eventTime > ctx.playbackState.strictValidUntilTimeInSamples)
            {
                continue;
            }

            const RenderedEventState renderedEventState{eventState, eventTime};

            ctx.playbackState.events.emplace_back(
                std::move(renderedEventState));
        }
    }
}

void computeSafeValidity(RenderContext& renderCtx,
                         const mpc::TimeInSamples safeValidUntilTimeInSamples,
                         const mpc::TimeInSamples safeValidFromTimeInSamples)
{
    const auto framesUntil =
        safeValidUntilTimeInSamples - renderCtx.playbackState.currentTimeInSamples;
    const auto framesFrom =
        safeValidFromTimeInSamples - renderCtx.playbackState.currentTimeInSamples;
    const int baseTick =
        Sequencer::quarterNotesToTicks(renderCtx.playbackState.playOffsetQuarterNotes);
    const int tickAdvanceUntil =
        SeqUtil::getTickCountForFrames(renderCtx.seq, baseTick, framesUntil,
                                       renderCtx.playbackState.sampleRate);
    const int tickAdvanceFrom =
        SeqUtil::getTickCountForFrames(renderCtx.seq, baseTick, framesFrom,
                                       renderCtx.playbackState.sampleRate);

    const mpc::Tick untilTick = baseTick + tickAdvanceUntil;
    const mpc::Tick fromTick = baseTick + tickAdvanceFrom;

    renderCtx.playbackState.safeValidUntilTimeInSamples = safeValidUntilTimeInSamples;
    renderCtx.playbackState.safeValidUntilTick = untilTick;
    renderCtx.playbackState.safeValidUntilQuarterNote =
        Sequencer::ticksToQuarterNotes(untilTick);

    renderCtx.playbackState.safeValidFromTimeInSamples = safeValidFromTimeInSamples;
    renderCtx.playbackState.safeValidFromTick = fromTick;
    renderCtx.playbackState.safeValidFromQuarterNote =
        Sequencer::ticksToQuarterNotes(fromTick);
}

PlaybackState NonRtSequencerStateWorker::renderPlaybackState(
    const SampleRate sampleRate, const PositionQuarterNotes playOffset,
    const TimeInSamples currentTime) const
{
    constexpr TimeInSamples snapshotWindowSize{44100 * 2};

    const TimeInSamples strictValidUntilTimeInSamples = currentTime + snapshotWindowSize;

    const auto seqIndex = sequencer->isSongModeEnabled()
                              ? sequencer->getSongSequenceIndex()
                              : sequencer->getSelectedSequenceIndex();

    const auto seq = sequencer->getSequence(seqIndex);

    PlaybackState playbackState{sequencer->getTransport()->isCountEnabled(),
                                sampleRate, playOffset, currentTime,
                                strictValidUntilTimeInSamples};

    RenderContext renderCtx{std::move(playbackState), sequencer, seq.get()};

    renderSeq(renderCtx);

    const bool isStepEditor =
        isCurrentScreen({lcdgui::ScreenId::StepEditorScreen});

    const auto countMetronomeScreen =
        sequencer->getScreens()
            ->get<lcdgui::ScreenId::CountMetronomeScreen>()
            .get();

    const MetronomeRenderContext mctx{countMetronomeScreen, isStepEditor,
                                      isRecMainWithoutPlaying()};

    renderMetronome(renderCtx, mctx);

    const auto safeValidUntilTimeInSamples =
    strictValidUntilTimeInSamples - playbackStateValiditySafetyMarginTimeInSamples;
    const auto safeValidFromTimeInSamples =
        currentTime + playbackStateValiditySafetyMarginTimeInSamples;

    computeSafeValidity(renderCtx, safeValidUntilTimeInSamples, safeValidFromTimeInSamples);

    return renderCtx.playbackState;
}
