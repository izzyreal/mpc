#include "sequencer/SequencerStateWorker.hpp"

#include "Event.hpp"
#include "MetronomeRenderer.hpp"
#include "RenderContext.hpp"
#include "SeqUtil.hpp"
#include "Sequence.hpp"
#include "Track.hpp"
#include "SequencerStateManager.hpp"
#include "SequencerAudioStateManager.hpp"
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

void SequencerStateWorker::work() const
{
    const auto snapshot = sequencer->getStateManager()->getSnapshot();
    const auto transportState = snapshot.getTransportState();

    const auto audioSnapshot = sequencer->getAudioStateManager()->getSnapshot();
    const auto currentTimeInSamples = audioSnapshot.getTimeInSamples();

    auto playbackState = snapshot.getPlaybackState();

    const auto seq = sequencer->getSelectedSequence();

    bool snapshotIsInvalid = false;

    if (transportState.getPositionQuarterNotes() != NoPositionQuarterNotes &&
        currentTimeInSamples >
            playbackState.strictValidUntilTimeInSamples -
                playbackStateValiditySafetyMarginTimeInSamples)
    {
        snapshotIsInvalid = true;
    }

    if (snapshotIsInvalid)
    {
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

void renderSeq(RenderContext &ctx)
{
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

            // event.printInfo();

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

    PlaybackState playbackState = previousPlaybackState;
    playbackState.sampleRate = sampleRate;

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
