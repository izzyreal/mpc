#include "engine/SequencerPlaybackEngine.hpp"

#include "engine/EngineHost.hpp"
#include "engine/NoteRepeatProcessor.hpp"

#include "sequencer/Transport.hpp"
#include "sequencer/MidiClockOutput.hpp"
#include "sequencer/Sequence.hpp"
#include "sequencer/SequencerStateManager.hpp"
#include "sequencer/Song.hpp"
#include "sequencer/Clock.hpp"

#include "lcdgui/LayeredScreen.hpp"
#include "lcdgui/screens/SyncScreen.hpp"
#include "lcdgui/screens/window/TimingCorrectScreen.hpp"
#include "lcdgui/screens/window/CountMetronomeScreen.hpp"
#include "lcdgui/screens/UserScreen.hpp"
#include "lcdgui/screens/SequencerScreen.hpp"

#include "performance/PerformanceManager.hpp"
#include "performance/PerformanceMessage.hpp"

#ifdef VMPC_AUDIO_TELEMETRY
#include <array>
#include <algorithm>
#include <cstdio>
#include <cmath>
#include <thread>
#include <atomic>

#include "concurrency/BoundedMpmcQueue.hpp"

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#else
#include <chrono>
#endif
#endif

using namespace mpc::engine;
using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;
using namespace mpc::sequencer;

#ifdef VMPC_AUDIO_TELEMETRY
namespace
{
#ifdef _WIN32
    inline int64_t telemetryNow() noexcept
    {
        LARGE_INTEGER value;
        QueryPerformanceCounter(&value);
        return value.QuadPart;
    }

    inline double telemetryDeltaUs(const int64_t begin,
                                   const int64_t end) noexcept
    {
        static const double inverseFreqUs =
            []
            {
                LARGE_INTEGER freq;
                QueryPerformanceFrequency(&freq);
                return 1000000.0 / static_cast<double>(freq.QuadPart);
            }();

        return static_cast<double>(end - begin) * inverseFreqUs;
    }
#else
    inline int64_t telemetryNow() noexcept
    {
        using namespace std::chrono;
        return duration_cast<microseconds>(
                   steady_clock::now().time_since_epoch())
            .count();
    }

    inline double telemetryDeltaUs(const int64_t begin,
                                   const int64_t end) noexcept
    {
        return static_cast<double>(end - begin);
    }
#endif

    struct AudioTelemetry
    {
        static constexpr size_t SampleWindow = 256;
        static constexpr double ReportIntervalUs = 2000000.0;

        std::array<double, SampleWindow> recentWorkUs{};
        size_t recentWriteIndex = 0;
        size_t recentCount = 0;

        uint64_t totalBuffers = 0;
        uint64_t totalOverruns = 0;
        uint64_t totalTicks = 0;
        uint32_t maxTicksInBuffer = 0;

        double sumWorkUs = 0.0;
        double maxWorkUs = 0.0;
        double sumLoopUs = 0.0;
        double maxLoopUs = 0.0;
        double sumMidiClockUs = 0.0;
        double maxMidiClockUs = 0.0;
        double sumTriggerClickUs = 0.0;
        double maxTriggerClickUs = 0.0;
        double sumPlayTickUs = 0.0;
        double maxPlayTickUs = 0.0;
        double sumNoteRepeatUs = 0.0;
        double maxNoteRepeatUs = 0.0;
        double sumDrainUs = 0.0;
        double maxDrainUs = 0.0;
        double sumUiPostUs = 0.0;
        double maxUiPostUs = 0.0;
        uint64_t totalTracksVisited = 0;
        uint64_t totalPlayNextCalls = 0;

        int64_t lastReportCounter = telemetryNow();

        void pushSample(const double workUs, const double loopUs,
                        const double deadlineUs, const uint32_t tickCount,
                        const double midiClockUs, const double triggerClickUs,
                        const double playTickUs, const double noteRepeatUs,
                        const double drainUs, const double uiPostUs,
                        const uint32_t tracksVisited,
                        const uint32_t playNextCalls) noexcept
        {
            totalBuffers++;
            totalTicks += tickCount;
            maxTicksInBuffer = std::max(maxTicksInBuffer, tickCount);
            sumWorkUs += workUs;
            sumLoopUs += loopUs;
            maxWorkUs = std::max(maxWorkUs, workUs);
            maxLoopUs = std::max(maxLoopUs, loopUs);
            sumMidiClockUs += midiClockUs;
            maxMidiClockUs = std::max(maxMidiClockUs, midiClockUs);
            sumTriggerClickUs += triggerClickUs;
            maxTriggerClickUs = std::max(maxTriggerClickUs, triggerClickUs);
            sumPlayTickUs += playTickUs;
            maxPlayTickUs = std::max(maxPlayTickUs, playTickUs);
            sumNoteRepeatUs += noteRepeatUs;
            maxNoteRepeatUs = std::max(maxNoteRepeatUs, noteRepeatUs);
            sumDrainUs += drainUs;
            maxDrainUs = std::max(maxDrainUs, drainUs);
            sumUiPostUs += uiPostUs;
            maxUiPostUs = std::max(maxUiPostUs, uiPostUs);
            totalTracksVisited += tracksVisited;
            totalPlayNextCalls += playNextCalls;

            if (workUs > deadlineUs)
            {
                totalOverruns++;
            }

            recentWorkUs[recentWriteIndex] = workUs;
            recentWriteIndex = (recentWriteIndex + 1) % SampleWindow;
            recentCount = std::min(recentCount + 1, SampleWindow);
        }

        static double computeP99(const std::array<double, SampleWindow> &buffer,
                                 const size_t count) noexcept
        {
            if (count == 0)
            {
                return 0.0;
            }

            std::array<double, SampleWindow> scratch{};
            std::copy_n(buffer.begin(), count, scratch.begin());
            const size_t p99Index = static_cast<size_t>(
                std::ceil(static_cast<double>(count) * 0.99)) -
                1;
            std::nth_element(scratch.begin(), scratch.begin() + p99Index,
                             scratch.begin() + count);
            return scratch[p99Index];
        }

        bool maybeBuildReport(const int sampleRate, const int nFrames,
                              const double deadlineUs,
                              char (&outReport)[512]) noexcept
        {
            const auto now = telemetryNow();
            if (telemetryDeltaUs(lastReportCounter, now) < ReportIntervalUs)
            {
                return false;
            }

            lastReportCounter = now;

            const double avgWorkUs =
                totalBuffers > 0 ? (sumWorkUs / static_cast<double>(totalBuffers))
                                 : 0.0;
            const double avgLoopUs =
                totalBuffers > 0 ? (sumLoopUs / static_cast<double>(totalBuffers))
                                 : 0.0;
            const double avgTicks =
                totalBuffers > 0 ? (static_cast<double>(totalTicks) /
                                    static_cast<double>(totalBuffers))
                                 : 0.0;
            const double avgMidiClockUs =
                totalBuffers > 0
                    ? (sumMidiClockUs / static_cast<double>(totalBuffers))
                    : 0.0;
            const double avgTriggerClickUs =
                totalBuffers > 0
                    ? (sumTriggerClickUs / static_cast<double>(totalBuffers))
                    : 0.0;
            const double avgPlayTickUs =
                totalBuffers > 0
                    ? (sumPlayTickUs / static_cast<double>(totalBuffers))
                    : 0.0;
            const double avgNoteRepeatUs =
                totalBuffers > 0
                    ? (sumNoteRepeatUs / static_cast<double>(totalBuffers))
                    : 0.0;
            const double avgDrainUs =
                totalBuffers > 0
                    ? (sumDrainUs / static_cast<double>(totalBuffers))
                    : 0.0;
            const double avgUiPostUs =
                totalBuffers > 0
                    ? (sumUiPostUs / static_cast<double>(totalBuffers))
                    : 0.0;
            const double avgTracksVisited =
                totalBuffers > 0
                    ? (static_cast<double>(totalTracksVisited) /
                       static_cast<double>(totalBuffers))
                    : 0.0;
            const double avgPlayNextCalls =
                totalBuffers > 0
                    ? (static_cast<double>(totalPlayNextCalls) /
                       static_cast<double>(totalBuffers))
                    : 0.0;
            const double overrunPercent =
                totalBuffers > 0
                    ? (100.0 * static_cast<double>(totalOverruns) /
                       static_cast<double>(totalBuffers))
                    : 0.0;
            const double p99WorkUs = computeP99(recentWorkUs, recentCount);
            const double p99HeadroomUs = deadlineUs - p99WorkUs;

            std::snprintf(
                outReport, sizeof(outReport),
                "[AUDIO_TELEMETRY] sr=%d bs=%d deadline_us=%.2f buffers=%llu "
                "avg_us=%.2f p99_us=%.2f max_us=%.2f avg_loop_us=%.2f "
                "max_loop_us=%.2f avg_midi_us=%.2f max_midi_us=%.2f "
                "avg_click_us=%.2f max_click_us=%.2f avg_playtick_us=%.2f "
                "max_playtick_us=%.2f avg_note_repeat_us=%.2f "
                "max_note_repeat_us=%.2f avg_drain_us=%.2f max_drain_us=%.2f "
                "avg_ui_post_us=%.2f max_ui_post_us=%.2f "
                "avg_tracks_visited=%.2f avg_playnext_calls=%.2f "
                "p99_headroom_us=%.2f overruns=%llu (%.2f%%) avg_ticks=%.2f "
                "max_ticks=%u\n",
                sampleRate, nFrames, deadlineUs,
                static_cast<unsigned long long>(totalBuffers), avgWorkUs,
                p99WorkUs, maxWorkUs, avgLoopUs, maxLoopUs, avgMidiClockUs,
                maxMidiClockUs, avgTriggerClickUs, maxTriggerClickUs,
                avgPlayTickUs, maxPlayTickUs, avgNoteRepeatUs, maxNoteRepeatUs,
                avgDrainUs, maxDrainUs, avgUiPostUs, maxUiPostUs,
                avgTracksVisited, avgPlayNextCalls, p99HeadroomUs,
                static_cast<unsigned long long>(totalOverruns), overrunPercent,
                avgTicks, maxTicksInBuffer);
            return true;
        }
    };

    AudioTelemetry &audioTelemetry() noexcept
    {
        static AudioTelemetry instance;
        return instance;
    }

    struct TelemetryLine
    {
        char text[512];
    };

    class TelemetryLogger
    {
    public:
        TelemetryLogger() : running(true), worker([this] { run(); })
        {
            enqueue("[AUDIO_TELEMETRY] logger_thread_started\n");
        }

        ~TelemetryLogger()
        {
            running.store(false, std::memory_order_release);
            if (worker.joinable())
            {
                worker.join();
            }
        }

        void enqueue(const char *line) noexcept
        {
            TelemetryLine message{};
            std::snprintf(message.text, sizeof(message.text), "%s", line);
            queue.enqueue(std::move(message));
        }

    private:
        static constexpr size_t QueueCapacity = 128;
        mpc::concurrency::BoundedMpmcQueue<TelemetryLine, QueueCapacity> queue;
        std::atomic<bool> running{false};
        std::thread worker;

        void run() noexcept
        {
            while (running.load(std::memory_order_acquire))
            {
                TelemetryLine line{};
                while (queue.dequeue(line))
                {
                    std::fputs(line.text, stdout);
                    std::fflush(stdout);
#ifdef _WIN32
                    OutputDebugStringA(line.text);
                    if (auto *log = std::fopen("vmpc.log", "a"))
                    {
                        std::fputs(line.text, log);
                        std::fclose(log);
                    }
#endif
                }

#ifdef _WIN32
                Sleep(2);
#else
                std::this_thread::sleep_for(std::chrono::milliseconds(2));
#endif
            }

            TelemetryLine line{};
            while (queue.dequeue(line))
            {
                std::fputs(line.text, stdout);
                std::fflush(stdout);
#ifdef _WIN32
                OutputDebugStringA(line.text);
                if (auto *log = std::fopen("vmpc.log", "a"))
                {
                    std::fputs(line.text, log);
                    std::fclose(log);
                }
#endif
            }
        }
    };

    TelemetryLogger &telemetryLogger() noexcept
    {
        static TelemetryLogger instance;
        return instance;
    }
} // namespace
#endif

SequencerPlaybackEngine::SequencerPlaybackEngine(
    performance::PerformanceManager *performanceManager, EngineHost *engineHost,
    std::function<std::shared_ptr<audiomidi::MidiOutput>()> getMidiOutput,
    Sequencer *sequencer, const std::shared_ptr<Clock> &clock,
    const std::shared_ptr<LayeredScreen> &layeredScreen,
    std::function<bool()> isBouncing, const std::function<int()> &getSampleRate,
    const std::function<bool()> &isRecMainWithoutPlaying,
    const std::function<void(int velo, int frameOffset)> &playMetronome,
    std::function<std::shared_ptr<Screens>()> getScreens,
    const std::function<bool()> &isNoteRepeatLockedOrPressed,
    const std::shared_ptr<NoteRepeatProcessor> &noteRepeatProcessor,
    const std::function<bool()> &isAudioServerCurrentlyRunningOffline)
    : performanceManager(performanceManager), engineHost(engineHost),
      layeredScreen(layeredScreen), getScreens(getScreens),
      sequencer(sequencer), clock(clock), isBouncing(isBouncing),
      getSampleRate(getSampleRate),
      isRecMainWithoutPlaying(isRecMainWithoutPlaying),
      playMetronome(playMetronome),
      isNoteRepeatLockedOrPressed(isNoteRepeatLockedOrPressed),
      noteRepeatProcessor(noteRepeatProcessor),
      isAudioServerCurrentlyRunningOffline(
          isAudioServerCurrentlyRunningOffline),
      midiClockOutput(std::make_shared<MidiClockOutput>(
          engineHost, getSampleRate, getMidiOutput, sequencer, getScreens,
          isBouncing))
{
#ifdef VMPC_AUDIO_TELEMETRY
    telemetryLogger();
#endif
}

unsigned short SequencerPlaybackEngine::getEventFrameOffset() const
{
    return tickFrameOffset;
}

void SequencerPlaybackEngine::setTickPositionEffectiveImmediately(
    const int newTickPos, const SequenceIndex sequenceIndex) const
{
    sequencer->getTransport()->setPositionImmediateWithoutPublish(
        Sequencer::ticksToQuarterNotes(newTickPos));

    const auto manager = sequencer->getStateManager();
    manager->drainQueueWithoutPublish();
    manager->enqueue(SyncTrackEventIndices{sequenceIndex});
    manager->drainQueueWithoutPublish();
}

std::shared_ptr<Sequence> SequencerPlaybackEngine::switchToNextSequence() const
{
    sequencer->playTick(sequencer->getTransport()->getTickPosition());
    const auto nextSequenceIndex = sequencer->getNextSq();
    sequencer->setSelectedSequenceIndex(nextSequenceIndex, false);
    sequencer->getStateManager()->drainQueueWithoutPublish();
    sequencer->clearNextSq();
    sequencer->getStateManager()->drainQueueWithoutPublish();
    setTickPositionEffectiveImmediately(0, nextSequenceIndex);
    return sequencer->getCurrentlyPlayingSequence();
}

void SequencerPlaybackEngine::triggerClickIfNeeded() const
{
    const auto transport =
        sequencer->getStateManager()->getSnapshot().getTransportStateView();

    if (!transport.isCountEnabled())
    {
        return;
    }

    const bool isStepEditor =
        layeredScreen->isCurrentScreen({ScreenId::StepEditorScreen});

    const auto currentScreenName = layeredScreen->getCurrentScreenName();

    const auto countMetronomeScreen =
        getScreens()->get<ScreenId::CountMetronomeScreen>();

    if (transport.isRecordingOrOverdubbing())
    {
        if (!countMetronomeScreen->getInRec() && !transport.isCountingIn())
        {
            return;
        }
    }
    else
    {
        if (!isStepEditor && !countMetronomeScreen->getInPlay() &&
            !transport.isCountingIn() && !isRecMainWithoutPlaying())
        {
            return;
        }
    }

    const auto pos = transport.isMetronomeOnlyEnabled()
                         ? transport.getMetronomeOnlyPositionTicks()
                         : transport.getPositionTicks();

    const auto bar = sequencer->getTransport()->getCurrentBarIndex();
    const auto seq = sequencer->getCurrentlyPlayingSequence();
    const auto firstTickOfBar = seq->getFirstTickOfBar(bar);
    const auto relativePos = pos - firstTickOfBar;

    if ((isStepEditor || isRecMainWithoutPlaying()) && relativePos == 0)
    {
        return;
    }

    const auto den = seq->getDenominator(bar);
    auto denTicks = 96 * (4.0 / den);

    switch (countMetronomeScreen->getRate())
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

    if (relativePos % static_cast<int>(denTicks) == 0)
    {
        playMetronome(relativePos == 0 ? 127 : 64, getEventFrameOffset());
    }
}

void SequencerPlaybackEngine::displayPunchRects() const
{
    const auto punch = sequencer->getTransport()->isPunchEnabled() &&
                       sequencer->getTransport()->isRecordingOrOverdubbing();

    if (punch)
    {
        const bool punchIn =
            sequencer->getTransport()->getAutoPunchMode() == 0 ||
            sequencer->getTransport()->getAutoPunchMode() == 2;
        const bool punchOut =
            sequencer->getTransport()->getAutoPunchMode() == 1 ||
            sequencer->getTransport()->getAutoPunchMode() == 2;
        const auto punchInTime = sequencer->getTransport()->getPunchInTime();
        const auto punchOutTime = sequencer->getTransport()->getPunchOutTime();

        const auto sequencerScreen =
            getScreens()->get<ScreenId::SequencerScreen>();

        if (punchIn &&
            sequencer->getTransport()->getTickPosition() == punchInTime)
        {
            sequencerScreen->setPunchRectOn(0, false);
            sequencerScreen->setPunchRectOn(1, true);
        }
        else if (punchOut &&
                 sequencer->getTransport()->getTickPosition() == punchOutTime)
        {
            sequencerScreen->setPunchRectOn(1, false);
            sequencerScreen->setPunchRectOn(2, true);
        }
    }
}

void SequencerPlaybackEngine::stopCountingInIfRequired(
    const SequenceIndex sequenceIndex) const
{
    if (const auto transport = sequencer->getTransport();
        transport->getTickPosition() >= transport->getCountInEndPosTicks())
    {
        setTickPositionEffectiveImmediately(
            transport->getCountInStartPosTicks(), sequenceIndex);
        transport->setCountingIn(false);
        sequencer->getStateManager()->drainQueueWithoutPublish();
    }
}

// Returns true if the song has stopped
bool SequencerPlaybackEngine::processSongMode() const
{
    const auto seq = sequencer->getCurrentlyPlayingSequence();

    if (sequencer->getTransport()->getTickPosition() < seq->getLastTick())
    {
        return false;
    }

    sequencer->playTick(seq->getLastTick() - 1);
    sequencer->getTransport()->incrementPlayedStepRepetitions();
    sequencer->getStateManager()->drainQueueWithoutPublish();
    const auto song = sequencer->getSelectedSong();
    const auto stepIndex = sequencer->getSelectedSongStepIndex();
    const auto step = song->getStep(stepIndex);

    const auto doneRepeating =
        sequencer->getTransport()->getPlayedStepRepetitions() >=
        step.repetitionCount;

    const auto reachedLastStep = stepIndex == song->getStepCount() - 1;

    if (doneRepeating && song->isLoopEnabled() &&
        stepIndex == song->getLastLoopStepIndex())
    {
        sequencer->getTransport()->resetPlayedStepRepetitions();
        sequencer->setSelectedSongStepIndex(song->getFirstLoopStepIndex());
        sequencer->getStateManager()->drainQueueWithoutPublish();

        if (const auto newStep = song->getStep(song->getFirstLoopStepIndex());
            !sequencer->getSequence(newStep.sequenceIndex)->isUsed())
        {
            stopSequencer();
            return true;
        }
        else
        {
            setTickPositionEffectiveImmediately(0, newStep.sequenceIndex);
        }
    }
    else if (doneRepeating && reachedLastStep)
    {
        sequencer->getTransport()->setEndOfSong(true);
        stopSequencer();
        return true;
    }
    else
    {
        if (doneRepeating)
        {
            sequencer->getTransport()->resetPlayedStepRepetitions();
            sequencer->setSelectedSongStepIndex(stepIndex + 1);

            if (const auto newStep = song->getStep(stepIndex + 1);
                !sequencer->getSequence(newStep.sequenceIndex)->isUsed())
            {
                stopSequencer();
                return true;
            }
        }
        else
        {
            sequencer->playTick(seq->getLastTick() - 1);
        }

        setTickPositionEffectiveImmediately(0, seq->getSequenceIndex());
    }

    return false;
}

bool SequencerPlaybackEngine::processSeqLoopEnabled() const
{
    if (const auto seq = sequencer->getCurrentlyPlayingSequence();
        sequencer->getTransport()->getTickPosition() >=
        seq->getLoopEndTick() - 1)
    {
        const auto punch =
            sequencer->getTransport()->isPunchEnabled() &&
            sequencer->getTransport()->isRecordingOrOverdubbing();
        const bool punchIn =
            sequencer->getTransport()->getAutoPunchMode() == 0 ||
            sequencer->getTransport()->getAutoPunchMode() == 2;
        const bool punchOut =
            sequencer->getTransport()->getAutoPunchMode() == 1 ||
            sequencer->getTransport()->getAutoPunchMode() == 2;

        const auto sequencerScreen =
            getScreens()->get<ScreenId::SequencerScreen>();

        if (punch && punchIn)
        {
            sequencerScreen->setPunchRectOn(0, true);
            sequencerScreen->setPunchRectOn(1, false);
        }

        if (punch && punchOut)
        {
            sequencerScreen->setPunchRectOn(2, false);
        }

        if (punch && punchOut && !punchIn)
        {
            sequencerScreen->setPunchRectOn(1, true);
        }

        sequencer->playTick(sequencer->getTransport()->getTickPosition());
        setTickPositionEffectiveImmediately(seq->getLoopStartTick(),
                                            seq->getSequenceIndex());

        if (sequencer->getTransport()->isRecording())
        {
            sequencer->getTransport()->switchRecordToOverdub();
        }

        return true;
    }

    return false;
}

bool SequencerPlaybackEngine::processSeqLoopDisabled() const
{
    if (const auto seq = sequencer->getCurrentlyPlayingSequence();
        sequencer->getTransport()->getTickPosition() >= seq->getLastTick())
    {
        if (sequencer->getTransport()->isRecordingOrOverdubbing())
        {
            const auto userScreen = getScreens()->get<ScreenId::UserScreen>();

            seq->insertBars(1, BarIndex(seq->getLastBarIndex()));
            seq->setTimeSignature(
                seq->getLastBarIndex(), seq->getLastBarIndex(),
                userScreen->timeSig.numerator, userScreen->timeSig.denominator);
        }
        else
        {
            sequencer->getTransport()->stop();
            setTickPositionEffectiveImmediately(
                Sequencer::ticksToQuarterNotes(seq->getLastTick()),
                seq->getSequenceIndex());
        }

        return true;
    }

    return false;
}

void SequencerPlaybackEngine::processNoteRepeat() const
{
    if (!layeredScreen->isCurrentScreen({ScreenId::SequencerScreen}))
    {
        return;
    }

    if (!isNoteRepeatLockedOrPressed())
    {
        return;
    }

    const auto timingCorrectScreen =
        getScreens()->get<ScreenId::TimingCorrectScreen>();

    const auto repeatIntervalTicks =
        timingCorrectScreen->getNoteValueLengthInTicks();
    const int swingPercentage = timingCorrectScreen->getSwing();
    const int swingOffset = static_cast<int>(
        (swingPercentage - 50) * (4.0 * 0.01) * (repeatIntervalTicks * 0.5));
    const auto shiftTiming =
        timingCorrectScreen->getAmount() *
        (timingCorrectScreen->isShiftTimingLater() ? 1 : -1);
    const auto tickPosWithShift =
        sequencer->getTransport()->getTickPosition() - shiftTiming;

    bool shouldRepeatNote = false;

    if (repeatIntervalTicks == 24 || repeatIntervalTicks == 48)
    {
        if (tickPosWithShift % (repeatIntervalTicks * 2) ==
                swingOffset + repeatIntervalTicks ||
            tickPosWithShift % (repeatIntervalTicks * 2) == 0)
        {
            shouldRepeatNote = true;
        }
    }
    else if (repeatIntervalTicks != 1 &&
             tickPosWithShift % repeatIntervalTicks == 0)
    {
        shouldRepeatNote = true;
    }

    if (shouldRepeatNote)
    {
        performanceManager->enqueue(performance::ActivateQuantizedLock{});
        noteRepeatProcessor->process(
            engineHost, sequencer->getTransport()->getTickPosition(),
            repeatIntervalTicks, getEventFrameOffset(),
            sequencer->getTransport()->getTempo(),
            static_cast<float>(getSampleRate()));
    }
}

void SequencerPlaybackEngine::stopSequencer() const
{
    const auto seq = sequencer->getCurrentlyPlayingSequence();
    sequencer->getTransport()->stop();
    setTickPositionEffectiveImmediately(0, seq->getSequenceIndex());
}

void SequencerPlaybackEngine::work(const int nFrames)
{
    // printf("BUFFER\n");
#ifdef VMPC_AUDIO_TELEMETRY
    const auto telemetryStart = telemetryNow();
    double loopWorkUs = 0.0;
    uint32_t processedTickCount = 0;
    double midiClockPhaseUs = 0.0;
    double triggerClickPhaseUs = 0.0;
    double playTickPhaseUs = 0.0;
    double noteRepeatPhaseUs = 0.0;
    double drainPhaseUs = 0.0;
    double uiPostPhaseUs = 0.0;
    uint32_t tracksVisitedCount = 0;
    uint32_t playNextCallsCount = 0;
#endif

    const auto manager = sequencer->getStateManager();
    const auto realtimeStateAccess = manager->scopedRealtimeStateAccess();
    manager->drainQueue();

    const auto sequencerSnapshot = manager->getSnapshot();
    const auto transportSnapshot = sequencerSnapshot.getTransportStateView();

    const bool sequencerIsRunningAtStartOfBuffer =
        transportSnapshot.isSequencerRunning();
    const auto sampleRate = getSampleRate();

    if (sequencerIsRunningAtStartOfBuffer &&
        transportSnapshot.isMetronomeOnlyEnabled())
    {
        clock->processBufferInternal(sequencer->getTransport()->getTempo(),
                                     sampleRate, nFrames, 0);
        const auto &ticksForCurrentBuffer = clock->getTicksForCurrentBuffer();

        size_t tickCursor = 0;
        for (uint16_t frameIndex = 0; frameIndex < nFrames; frameIndex++)
        {
            while (tickCursor < ticksForCurrentBuffer.size() &&
                   ticksForCurrentBuffer[tickCursor] == frameIndex)
            {
#ifdef VMPC_AUDIO_TELEMETRY
                const auto clickStart = telemetryNow();
#endif
                triggerClickIfNeeded();
#ifdef VMPC_AUDIO_TELEMETRY
                triggerClickPhaseUs += telemetryDeltaUs(clickStart, telemetryNow());
#endif
                manager->enqueue(BumpMetronomeOnlyTickPositionOneTick{});
#ifdef VMPC_AUDIO_TELEMETRY
                const auto drainStart = telemetryNow();
#endif
                manager->drainQueueWithoutPublish();
#ifdef VMPC_AUDIO_TELEMETRY
                drainPhaseUs += telemetryDeltaUs(drainStart, telemetryNow());
#endif
                tickCursor++;
            }
        }
#ifdef VMPC_AUDIO_TELEMETRY
        processedTickCount = static_cast<uint32_t>(ticksForCurrentBuffer.size());
#endif

#ifdef VMPC_AUDIO_TELEMETRY
        const auto loopEnd = telemetryNow();
        loopWorkUs = telemetryDeltaUs(telemetryStart, loopEnd);
#endif
        manager->publishActiveState();
        clock->clearTicks();

#ifdef VMPC_AUDIO_TELEMETRY
        const auto telemetryEnd = telemetryNow();
        const auto workUs = telemetryDeltaUs(telemetryStart, telemetryEnd);
        const auto deadlineUs = (sampleRate > 0)
                                    ? (1000000.0 * static_cast<double>(nFrames) /
                                       static_cast<double>(sampleRate))
                                    : 0.0;
        audioTelemetry().pushSample(
            workUs, loopWorkUs, deadlineUs, processedTickCount, midiClockPhaseUs,
            triggerClickPhaseUs, playTickPhaseUs, noteRepeatPhaseUs, drainPhaseUs,
            uiPostPhaseUs, tracksVisitedCount, playNextCallsCount);
        char telemetryReport[512]{};
        if (audioTelemetry().maybeBuildReport(sampleRate, nFrames, deadlineUs,
                                              telemetryReport))
        {
            telemetryLogger().enqueue(telemetryReport);
        }
#endif
        return;
    }

    if (sequencerIsRunningAtStartOfBuffer &&
        isAudioServerCurrentlyRunningOffline())
    {
        clock->processBufferInternal(
            sequencer->getTransport()->getTempo(), sampleRate, nFrames,
            sequencer->getTransport()->getPlayStartPositionQuarterNotes());
    }

    const auto &clockTicks = clock->getTicksForCurrentBuffer();
    const bool positionalJumpOccurred = clock->didJumpOccurInLastBuffer();

    auto seq = sequencer->getCurrentlyPlayingSequence();

    if (positionalJumpOccurred)
    {
        const auto hostPositionQuarterNotes =
            clock->getLastProcessedHostPositionQuarterNotes();

        if (sequencer->isSongModeEnabled())
        {
            const auto wrappedPosition =
                sequencer->getTransport()->getWrappedPositionInSong(
                    hostPositionQuarterNotes);
            sequencer->setSelectedSongStepIndex(wrappedPosition.stepIndex);
            sequencer->getTransport()->setPosition(wrappedPosition.position);
            sequencer->getStateManager()->enqueue(
                SetPlayedSongStepRepetitionCount{
                    wrappedPosition.playedRepetitionCount});
        }
        else
        {
            const auto wrappedPosition =
                sequencer->getTransport()->getWrappedPositionInSequence(
                    hostPositionQuarterNotes);

            sequencer->getTransport()->setPosition(wrappedPosition);
        }

        manager->drainQueueWithoutPublish();
        seq = sequencer->getCurrentlyPlayingSequence();
        manager->applyMessageImmediateWithoutPublish(
            SyncTrackEventIndices{seq->getSequenceIndex()});
    }

    bool songHasStopped = false;
    bool normalPlayHasStopped = false;

    midiClockOutput->processSampleRateChange();
    midiClockOutput->processTempoChange();

#ifdef VMPC_AUDIO_TELEMETRY
    const auto loopStart = telemetryNow();
#endif
    size_t tickCursor = 0;
    for (int frameIndex = 0; frameIndex < nFrames; frameIndex++)
    {
        size_t tickCountAtThisFrameIndex = 0;

        while (tickCursor < clockTicks.size() &&
               clockTicks[tickCursor] == frameIndex)
        {
            tickCountAtThisFrameIndex++;
            tickCursor++;
        }
#ifdef VMPC_AUDIO_TELEMETRY
        processedTickCount += static_cast<uint32_t>(tickCountAtThisFrameIndex);
#endif

#ifdef VMPC_AUDIO_TELEMETRY
        const auto midiStart = telemetryNow();
#endif
        midiClockOutput->processFrame(sequencerIsRunningAtStartOfBuffer,
                                      frameIndex, tickCountAtThisFrameIndex);
#ifdef VMPC_AUDIO_TELEMETRY
        midiClockPhaseUs += telemetryDeltaUs(midiStart, telemetryNow());
#endif

        if (!sequencerIsRunningAtStartOfBuffer)
        {
            tickFrameOffset = 0;
            continue;
        }

        if (const auto syncScreen = getScreens()->get<ScreenId::SyncScreen>();
            syncScreen->modeOut != 0 && !isBouncing())
        {
            if (midiClockOutput->isLastProcessedFrameMidiClockLock())
            {
                sequencer->getTransport()->setShouldWaitForMidiClockLock(false);
            }
            else if (sequencer->getTransport()->shouldWaitForMidiClockLock())
            {
                continue;
            }
        }

        if (tickCountAtThisFrameIndex == 0)
        {
            continue;
        }

        if (tickCountAtThisFrameIndex > 1)
        {
            sequencer->getTransport()->bumpPositionByTicksImmediateWithoutPublish(
                tickCountAtThisFrameIndex - 1);
        }

        tickFrameOffset = frameIndex;

#ifdef VMPC_AUDIO_TELEMETRY
        const auto clickStart = telemetryNow();
#endif
        triggerClickIfNeeded();
#ifdef VMPC_AUDIO_TELEMETRY
        triggerClickPhaseUs += telemetryDeltaUs(clickStart, telemetryNow());
#endif

#ifdef VMPC_AUDIO_TELEMETRY
        const auto uiPostStart = telemetryNow();
#endif
        layeredScreen->postToUiThread(utils::Task(
            [this]
            {
                displayPunchRects();
            }));
#ifdef VMPC_AUDIO_TELEMETRY
        uiPostPhaseUs += telemetryDeltaUs(uiPostStart, telemetryNow());
#endif

        if (sequencer->getTransport()->isCountingIn())
        {
            sequencer->getTransport()->bumpPositionByTicksImmediateWithoutPublish(
                1);
            stopCountingInIfRequired(seq->getSequenceIndex());
            continue;
        }

        if (sequencer->getTransport()->getTickPosition() >=
                seq->getLastTick() - 1 &&
            !sequencer->isSongModeEnabled() &&
            sequencer->getNextSq() >= MinSequenceIndex)
        {
            seq = switchToNextSequence();
            continue;
        }

        if (sequencer->isSongModeEnabled())
        {
            if (!songHasStopped && processSongMode())
            {
                songHasStopped = true;
                continue;
            }
        }
        else if (seq->isLoopEnabled())
        {
            if (processSeqLoopEnabled())
            {
                continue;
            }
        }
        else
        {
            if (!normalPlayHasStopped && processSeqLoopDisabled())
            {
                normalPlayHasStopped = true;
            }
        }

        if (!songHasStopped && !normalPlayHasStopped)
        {
#ifdef VMPC_AUDIO_TELEMETRY
            const auto playTickStart = telemetryNow();
#endif
#ifdef VMPC_AUDIO_TELEMETRY
            sequencer->playTick(sequencer->getTransport()->getTickPosition(),
                                &tracksVisitedCount, &playNextCallsCount);
#else
            sequencer->playTick(sequencer->getTransport()->getTickPosition());
#endif
#ifdef VMPC_AUDIO_TELEMETRY
            playTickPhaseUs += telemetryDeltaUs(playTickStart, telemetryNow());
            const auto noteRepeatStart = telemetryNow();
#endif
            processNoteRepeat();
#ifdef VMPC_AUDIO_TELEMETRY
            noteRepeatPhaseUs +=
                telemetryDeltaUs(noteRepeatStart, telemetryNow());
#endif
            sequencer->getTransport()->bumpPositionByTicksImmediateWithoutPublish(
                1);
        }

#ifdef VMPC_AUDIO_TELEMETRY
        const auto drainStart = telemetryNow();
#endif
        manager->drainQueueWithoutPublish();
#ifdef VMPC_AUDIO_TELEMETRY
        drainPhaseUs += telemetryDeltaUs(drainStart, telemetryNow());
#endif
    }
#ifdef VMPC_AUDIO_TELEMETRY
    const auto loopEnd = telemetryNow();
    loopWorkUs = telemetryDeltaUs(loopStart, loopEnd);
#endif

    manager->publishActiveState();
    clock->clearTicks();

#ifdef VMPC_AUDIO_TELEMETRY
    const auto telemetryEnd = telemetryNow();
    const auto workUs = telemetryDeltaUs(telemetryStart, telemetryEnd);
    const auto deadlineUs = (sampleRate > 0)
                                ? (1000000.0 * static_cast<double>(nFrames) /
                                   static_cast<double>(sampleRate))
                                : 0.0;
    audioTelemetry().pushSample(
        workUs, loopWorkUs, deadlineUs, processedTickCount, midiClockPhaseUs,
        triggerClickPhaseUs, playTickPhaseUs, noteRepeatPhaseUs, drainPhaseUs,
        uiPostPhaseUs, tracksVisitedCount, playNextCallsCount);
    char telemetryReport[512]{};
    if (audioTelemetry().maybeBuildReport(sampleRate, nFrames, deadlineUs,
                                          telemetryReport))
    {
        telemetryLogger().enqueue(telemetryReport);
    }
#endif
}
