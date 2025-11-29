#include "sequencer/SequencerStateWorker.hpp"

#include "Event.hpp"
#include "MetronomeRenderer.hpp"
#include "PlaybackStateValidity.hpp"
#include "RenderContext.hpp"
#include "Sequence.hpp"
#include "SequenceRenderer.hpp"
#include "SequenceStateView.hpp"
#include "Track.hpp"
#include "SequencerStateManager.hpp"
#include "SequencerAudioStateManager.hpp"
#include "Transport.hpp"
#include "engine/SequencerPlaybackEngine.hpp"
#include "lcdgui/ScreenComponent.hpp"
#include "sequencer/Sequencer.hpp"
#include "utils/SequencerTiming.hpp"

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
    const auto sequencerStateView = sequencer->getStateManager()->getSnapshot();
    const auto transportState = sequencerStateView.getTransportStateView();

    const auto audioStateView = sequencer->getAudioStateManager()->getSnapshot();
    const auto currentTimeInSamples = audioStateView.getTimeInSamples();

    auto playbackState = sequencerStateView.getPlaybackState();

    bool snapshotIsInvalid = false;

    if (transportState.getPositionQuarterNotes() != NoPositionQuarterNotes &&
        currentTimeInSamples > playbackState.safeValidUntil)
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
        for (const auto &t : sequencer->getSelectedSequence()->getTracks())
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

    const auto sequencerStateView = sequencer->getStateManager()->getSnapshot();
    const auto playbackState =
        renderPlaybackState(SampleRate(sampleRate), previousPlaybackState,
                            timeInSamplesToUse, sequencerStateView);

    sequencer->getStateManager()->enqueue(
        UpdatePlaybackState{std::move(playbackState), onComplete});
}

Sequencer *SequencerStateWorker::getSequencer() const
{
    return sequencer;
}

void printRenderDebugInfo(const PlaybackState &prev,
                          const PlaybackState &current)
{
    static int counter = 0;
    printf("===================\n");
    printf("Current time: %lld\n", current.strictValidFrom);
    // printf("%i Rendering with previous PlaybackState:\n", counter);
    // prev.printOrigin();
    // prev.transition.printInfo();
    // printf("===================\n");
    printf("%i PlaybackState for this block:\n", counter);
    current.printOrigin();
    current.transition.printInfo();
    counter++;
}

void installTransition(RenderContext &ctx)
{
    if (!ctx.seq->isLoopEnabled())
    {
        return;
    }

    const auto transitionTick = ctx.seq->getLoopEndTick();
    const auto &state = ctx.playbackState;

    const auto currentTick = state.getCurrentTick(*ctx.seq, ctx.currentTime);

    const auto blockSizeTicks = state.strictLengthInTicks();

    if (currentTick + blockSizeTicks >= transitionTick)
    {
        const auto toTick = ctx.seq->getLoopStartTick();
        const double ticksUntilTransition = transitionTick - currentTick;

        const auto sequenceTimingData =
            mpc::utils::getSequenceTimingData(*ctx.seq);

        int transitionFrame = mpc::utils::getFrameCountForTicks(
            sequenceTimingData, currentTick, ticksUntilTransition,
            ctx.playbackState.sampleRate);

        transitionFrame += ctx.currentTime;

        ctx.playbackState.transition.activate(transitionTick, toTick,
                                              transitionFrame);
    }
}

PlaybackState initPlaybackState(const PlaybackState &prev,
                                const mpc::SampleRate sampleRate)
{
    PlaybackState playbackState = prev;
    playbackState.sampleRate = sampleRate;
    return playbackState;
}

RenderContext initRenderCtx(const PlaybackState &prevState,
                            const mpc::SampleRate sampleRate,
                            const mpc::TimeInSamples currentTime,
                            const SequencerStateView &sequencerStateView)
{
    PlaybackState playbackState = initPlaybackState(prevState, sampleRate);

    if (playbackState.transition.isActive() &&
        playbackState.transition.timeInSamples < currentTime)
    {
        playbackState.lastTransitionTick = prevState.transition.toTick;
        playbackState.lastTransitionTime = prevState.transition.timeInSamples;
        playbackState.transition.deactivate();
    }

    const auto seqIndex = sequencerStateView.getSelectedSequenceIndex();
    const auto seqState = sequencerStateView.getSequenceState(seqIndex);

    RenderContext renderCtx{std::move(playbackState),
                            sequencerStateView.getTransportStateView(),
                            seqState, currentTime};

    computeValidity(renderCtx, currentTime);

    return renderCtx;
}

PlaybackState SequencerStateWorker::renderPlaybackState(
    const SampleRate sampleRate, const PlaybackState &prevState,
    const TimeInSamples currentTime,
    const SequencerStateView &sequencerStateView) const
{
    auto renderCtx =
        initRenderCtx(prevState, sampleRate, currentTime, sequencerStateView);

    installTransition(renderCtx);

    printRenderDebugInfo(prevState, renderCtx.playbackState);

    renderSeq(renderCtx);

    const auto mctx =
        initMetronomeRenderContext(isCurrentScreen, isRecMainWithoutPlaying, sequencer->getScreens);

    renderMetronome(renderCtx, mctx);

    return renderCtx.playbackState;
}
