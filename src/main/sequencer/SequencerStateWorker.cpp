#include "sequencer/SequencerStateWorker.hpp"

#include "Event.hpp"
#include "MetronomeRenderer.hpp"
#include "PlaybackStateValidity.hpp"
#include "RenderContext.hpp"
#include "Sequence.hpp"
#include "SequenceRenderer.hpp"
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

void printRenderDebugInfo(const PlaybackState &s)
{
    static int counter = 0;
    printf("===================\n");
    printf("%i Rendering with previous PlaybackState:\n", counter++);
    s.printOrigin();
    printf("===================\n");
}

void installTransition(RenderContext &ctx)
{

}

PlaybackState initPlaybackState(const PlaybackState &prev,
                                const mpc::SampleRate sampleRate,
                                const mpc::TimeInSamples currentTime)
{
    constexpr mpc::TimeInSamples snapshotWindowSize{44100 * 2};

    PlaybackState playbackState = prev;
    playbackState.sampleRate = sampleRate;
    playbackState.strictValidFrom = currentTime;
    playbackState.strictValidUntil = currentTime + snapshotWindowSize;
    return playbackState;
}

RenderContext initRenderCtx(const PlaybackState &prevState,
                            const mpc::SampleRate sampleRate,
                            const mpc::TimeInSamples currentTime,
                            Sequencer *sequencer)
{
    PlaybackState playbackState =
        initPlaybackState(prevState, sampleRate, currentTime);

    RenderContext renderCtx{std::move(playbackState), sequencer,
                            sequencer->getCurrentlyPlayingSequence().get()};

    computeSafeValidity(renderCtx, currentTime);

    return renderCtx;
}

PlaybackState
SequencerStateWorker::renderPlaybackState(const SampleRate sampleRate,
                                          const PlaybackState &prevState,
                                          const TimeInSamples currentTime) const
{
    printRenderDebugInfo(prevState);

    auto renderCtx =
        initRenderCtx(prevState, sampleRate, currentTime, sequencer);

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

    return renderCtx.playbackState;
}
