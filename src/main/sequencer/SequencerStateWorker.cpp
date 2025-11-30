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
        return;

    running.store(true);

    if (workerThread.joinable())
        workerThread.join();

    workerThread = std::thread([this]
    {
        constexpr auto sampleRate = SampleRate(44100);
        constexpr int bufferSize = 512;

        constexpr double blockMs = static_cast<double>(bufferSize) / sampleRate * 1000.0;

        while (running.load())
        {
            const auto t0 = std::chrono::steady_clock::now();

            work();

            const auto t1 = std::chrono::steady_clock::now();
            const double workMs =
                std::chrono::duration<double, std::milli>(t1 - t0).count();

            double sleepMs = blockMs - workMs;
            if (sleepMs < 0.0)
                sleepMs = 0.0;

            std::this_thread::sleep_for(
                std::chrono::duration<double, std::milli>(sleepMs));
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
    sequencer->getStateManager()->drainQueue();
    const auto sequencerStateView = sequencer->getStateManager()->getSnapshot();
    const auto transportState = sequencerStateView.getTransportStateView();

    const auto audioStateView = sequencer->getAudioStateManager()->getSnapshot();
    const auto currentTimeInSamples = audioStateView.getTimeInSamples();

    const auto playbackState = sequencerStateView.getPlaybackState();

    constexpr auto onComplete = [] {};
    refreshPlaybackState(playbackState, currentTimeInSamples, onComplete);

    if (transportState.isRecordingOrOverdubbing())
    {
        for (const auto &t : sequencer->getSelectedSequence()->getTracks())
            t->processRealtimeQueuedEvents();
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
    playbackState.events.clear();
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

    renderSeq(renderCtx);

    const auto mctx =
        initMetronomeRenderContext(isCurrentScreen, isRecMainWithoutPlaying, sequencer->getScreens);

    renderMetronome(renderCtx, mctx);

    return renderCtx.playbackState;
}
