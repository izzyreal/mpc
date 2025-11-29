#include "sequencer/SequencerStateWorker.hpp"

#include "Event.hpp"
#include "SeqUtil.hpp"
#include "Sequence.hpp"
#include "Track.hpp"
#include "SequencerStateManager.hpp"
#include "SequencerAudioStateManager.hpp"
#include "TimeSignature.hpp"
#include "Transport.hpp"
#include "engine/SequencerPlaybackEngine.hpp"
#include "lcdgui/ScreenComponent.hpp"
#include "lcdgui/Screens.hpp"
#include "lcdgui/screens/window/CountMetronomeScreen.hpp"
#include "sequencer/Sequencer.hpp"

using namespace mpc::sequencer;

SequencerStateWorker::SequencerStateWorker(
    const std::function<bool(std::initializer_list<lcdgui::ScreenId>)>
        &isCurrentScreen,
    const std::function<bool()> &isRecMainWithoutPlaying, Sequencer *sequencer)
    : running(false), isCurrentScreen(isCurrentScreen),
      isRecMainWithoutPlaying(isRecMainWithoutPlaying), sequencer(sequencer)
{
}

SequencerStateWorker::~SequencerStateWorker()
{
    stop();

    if (workerThread.joinable())
    {
        workerThread.join();
    }
}

void SequencerStateWorker::start()
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

void SequencerStateWorker::stop()
{
    if (!running.load())
    {
        return;
    }

    running.store(false);
}

void SequencerStateWorker::stopAndWaitUntilStopped()
{
    stop();

    if (workerThread.joinable())
    {
        workerThread.join();
    }
}

bool SequencerStateWorker::willLoopWithinSafetyMargin(
    const PlaybackState &ps, const TimeInSamples now) const
{
    const auto seq = sequencer->getSelectedSequence().get();
    const int lastTick = seq->getLastTick();

    const int currentTick = ps.getCurrentTick(seq, now);
    const int ticksRemaining = lastTick - currentTick;

    // convert safety margin to ticks
    const int ticksMargin = SeqUtil::getTickCountForFrames(
        seq, currentTick, playbackStateValiditySafetyMarginTimeInSamples,
        ps.sampleRate);

    return ticksRemaining < ticksMargin;
}

void SequencerStateWorker::work() const
{
    const auto snapshot = sequencer->getStateManager()->getSnapshot();
    const auto transportState = snapshot.getTransportState();

    const auto audioSnapshot = sequencer->getAudioStateManager()->getSnapshot();
    const auto currentTimeInSamples = audioSnapshot.getTimeInSamples();

    auto playbackState = snapshot.getPlaybackState();

    const auto seq = sequencer->getSelectedSequence();

    const bool impendingLoop =
        seq->isLoopEnabled() &&
        willLoopWithinSafetyMargin(playbackState, currentTimeInSamples);

    bool snapshotIsInvalid = false;

    if (transportState.getPositionQuarterNotes() != NoPositionQuarterNotes &&
        currentTimeInSamples >
            playbackState.strictValidUntilTimeInSamples -
                playbackStateValiditySafetyMarginTimeInSamples)
    {
        snapshotIsInvalid = true;
    }

    if (snapshotIsInvalid || impendingLoop)
    {
        if (impendingLoop) printf("impending loop!\n");
        constexpr auto onComplete = [] {};
        playbackState.events.clear();
        refreshPlaybackState(playbackState, currentTimeInSamples, onComplete);
    }

    if (transportState.isRecordingOrOverdubbing())
    {
        for (const auto &t : seq->getTracks())
        {
            t->processRealtimeQueuedEvents();
        }
    }

    sequencer->getStateManager()->drainQueue();
}

void SequencerStateWorker::refreshPlaybackState(
    const PlaybackState &previousPlaybackState,
    const TimeInSamples timeInSamples,
    const std::function<void()> &onComplete) const
{
    TimeInSamples timeInSamplesToUse = timeInSamples;

    if (timeInSamplesToUse == CurrentTimeInSamples)
    {
        timeInSamplesToUse =
            sequencer->getAudioStateManager()->getSnapshot().getTimeInSamples();
        // printf("Time in samples to use: %i\n", timeInSamplesToUse);
    }

    const auto playbackEngine = sequencer->getSequencerPlaybackEngine();
    const auto sampleRate = playbackEngine->getSampleRate();

    const auto playbackState = renderPlaybackState(
        SampleRate(sampleRate), previousPlaybackState, timeInSamplesToUse);

    sequencer->getStateManager()->enqueue(
        UpdatePlaybackState{std::move(playbackState), onComplete});
}

Sequencer *SequencerStateWorker::getSequencer() const
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
            const auto eventTick = j + barTickOffset;
            const auto eventTickToUse =
                eventTick - ctx.playbackState.originTicks;

            const auto eventTimeInSamples = SeqUtil::getEventTimeInSamples(
                ctx.seq, eventTickToUse,
                ctx.playbackState.strictValidFromTimeInSamples,
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
    //        ctx.seq->getName().c_str(),
    //        ctx.playbackState.currentTimeInSamples, ctx.playOffsetTicks);

    for (const auto &track : ctx.seq->getTracks())
    {
        for (const auto &event : track->getEventStates())
        {
            if (event.type == EventType::TempoChange)
            {
                continue;
            }

            const auto eventTickToUse =
                event.tick - ctx.playbackState.originTicks;

            const mpc::TimeInSamples eventTime = SeqUtil::getEventTimeInSamples(
                ctx.seq, eventTickToUse,
                ctx.playbackState.strictValidFromTimeInSamples,
                ctx.playbackState.sampleRate);

            if (!ctx.playbackState.containsTimeInSamplesStrict(eventTime))
            {
                continue;
            }

            event.printInfo();

            const RenderedEventState renderedEventState{event, eventTime};

            ctx.playbackState.events.emplace_back(
                std::move(renderedEventState));
        }
    }
}

void computeSafeValidity(RenderContext &renderCtx,
                         const mpc::TimeInSamples safeValidUntilTimeInSamples,
                         const mpc::TimeInSamples safeValidFromTimeInSamples)
{
    const auto framesUntil =
        safeValidUntilTimeInSamples -
        renderCtx.playbackState.strictValidFromTimeInSamples;
    const auto framesFrom =
        safeValidFromTimeInSamples -
        renderCtx.playbackState.strictValidFromTimeInSamples;
    const int baseTick = renderCtx.playbackState.originTicks;
    const int tickAdvanceUntil =
        SeqUtil::getTickCountForFrames(renderCtx.seq, baseTick, framesUntil,
                                       renderCtx.playbackState.sampleRate);
    const int tickAdvanceFrom =
        SeqUtil::getTickCountForFrames(renderCtx.seq, baseTick, framesFrom,
                                       renderCtx.playbackState.sampleRate);

    const mpc::Tick untilTick = baseTick + tickAdvanceUntil;
    const mpc::Tick fromTick = baseTick + tickAdvanceFrom;

    renderCtx.playbackState.safeValidUntilTimeInSamples =
        safeValidUntilTimeInSamples;
    renderCtx.playbackState.safeValidUntilTick = untilTick;
    renderCtx.playbackState.safeValidUntilQuarterNote =
        Sequencer::ticksToQuarterNotes(untilTick);

    renderCtx.playbackState.safeValidFromTimeInSamples =
        safeValidFromTimeInSamples;
    renderCtx.playbackState.safeValidFromTick = fromTick;
    renderCtx.playbackState.safeValidFromQuarterNote =
        Sequencer::ticksToQuarterNotes(fromTick);
}

PlaybackState SequencerStateWorker::renderPlaybackState(
    const SampleRate sampleRate, const PlaybackState &previousPlaybackState,
    const TimeInSamples currentTime) const
{
    constexpr TimeInSamples snapshotWindowSize{44100 * 2};

    const TimeInSamples strictValidUntilTimeInSamples =
        currentTime + snapshotWindowSize;

    const auto seqIndex = sequencer->isSongModeEnabled()
                              ? sequencer->getSongSequenceIndex()
                              : sequencer->getSelectedSequenceIndex();

    const auto seq = sequencer->getSequence(seqIndex);

    const auto lastTick = seq->getLastTick();

    PlaybackState playbackState = previousPlaybackState;
    playbackState.sampleRate = sampleRate;

    // --- Derive musical position from samples ---
    const auto originQN = playbackState.originQuarterNotes;
    const auto originTicks = Sequencer::quarterNotesToTicks(originQN);

    const auto deltaSamples = currentTime - playbackState.originSampleTime;

    // If initial call (start of playback): originSampleTime is uninitialized.
    // Detect this and set cleanly:
    if (playbackState.originSampleTime == NoTimeInSamples)
    {
        playbackState.originSampleTime = currentTime;
        playbackState.originQuarterNotes = originQN;
        playbackState.originTicks = originTicks;
    }

    // Compute tickNow from continuous time
    const int tickAdvance =
        SeqUtil::getTickCountForFrames(seq.get(), playbackState.originTicks,
                                       deltaSamples, playbackState.sampleRate);

    int tickNow = playbackState.originTicks + tickAdvance;

    printf("tickNow: %i\n", tickNow);
    if (tickNow >= lastTick)
    {
        tickNow %= lastTick;

        // Reset musical origin to wrapped position
        playbackState.originTicks = tickNow;
        playbackState.originQuarterNotes =
            Sequencer::ticksToQuarterNotes(tickNow);
        playbackState.originSampleTime = currentTime;
    }

    // Validity boundaries
    playbackState.strictValidFromTimeInSamples = currentTime;
    playbackState.strictValidUntilTimeInSamples = strictValidUntilTimeInSamples;

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
        strictValidUntilTimeInSamples -
        playbackStateValiditySafetyMarginTimeInSamples;
    const auto safeValidFromTimeInSamples =
        currentTime + playbackStateValiditySafetyMarginTimeInSamples;

    computeSafeValidity(renderCtx, safeValidUntilTimeInSamples,
                        safeValidFromTimeInSamples);

    return renderCtx.playbackState;
}
