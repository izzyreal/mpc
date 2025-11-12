#include "engine/SequencerPlaybackEngine.hpp"

#include "sequencer/Transport.hpp"

#include "engine/NoteRepeatProcessor.hpp"

#include "lcdgui/LayeredScreen.hpp"
#include "lcdgui/screens/SyncScreen.hpp"
#include "sequencer/Sequence.hpp"
#include "sequencer/SequencerStateManager.hpp"
#include "sequencer/Song.hpp"
#include "sequencer/Step.hpp"
#include "sequencer/Clock.hpp"

#include "lcdgui/screens/window/TimingCorrectScreen.hpp"
#include "lcdgui/screens/window/CountMetronomeScreen.hpp"
#include "lcdgui/screens/SongScreen.hpp"
#include "lcdgui/screens/UserScreen.hpp"
#include "lcdgui/screens/SequencerScreen.hpp"

#include "sequencer/MidiClockOutput.hpp"

#include <concurrentqueue.h>

using namespace mpc::engine;
using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;
using namespace mpc::sequencer;

SequencerPlaybackEngine::SequencerPlaybackEngine(
    Sequencer *sequencer,
    const std::shared_ptr<Clock> &clock,
    const std::shared_ptr<LayeredScreen> &layeredScreen,
    std::function<bool()> isBouncing, const std::function<int()> &getSampleRate,
    const std::function<bool()> &isRecMainWithoutPlaying,
    const std::function<void(int velo, int frameOffset)> &playMetronome,
    std::function<std::shared_ptr<Screens>()> getScreens,
    const std::function<bool()> &isNoteRepeatLockedOrPressed,
    const std::shared_ptr<NoteRepeatProcessor> &noteRepeatProcessor,
    std::function<bool()> isAudioServerCurrentlyRunningOffline)
    : layeredScreen(layeredScreen),
      getScreens(getScreens), sequencer(sequencer), clock(clock),
      isBouncing(isBouncing), getSampleRate(getSampleRate),
      isRecMainWithoutPlaying(isRecMainWithoutPlaying),
      playMetronome(playMetronome),
      isNoteRepeatLockedOrPressed(isNoteRepeatLockedOrPressed),
      noteRepeatProcessor(noteRepeatProcessor),
      isAudioServerCurrentlyRunningOffline(isAudioServerCurrentlyRunningOffline),
      midiClockOutput(
          std::make_shared<MidiClockOutput>(sequencer, getScreens, isBouncing))
{
    eventQueue = std::make_shared<EventQueue>(100);
    tempEventQueue.reserve(100);
}

void SequencerPlaybackEngine::start(const bool metronomeOnlyToUse)
{
    if (sequencerIsRunning.load())
    {
        return;
    }

    if (getScreens()->get<ScreenId::SyncScreen>()->modeOut != 0)
    {
        // TODO Implement MIDI clock out
        // shouldWaitForMidiClockLock = true;
        shouldWaitForMidiClockLock = false;
    }

    clock->reset();

    metronomeOnly = metronomeOnlyToUse;
    metronomeOnlyTickPosition = 0;

    sequencerIsRunning.store(true);
}

void SequencerPlaybackEngine::startMetronome()
{
    if (sequencerIsRunning.load())
    {
        return;
    }

    start(true);
}

unsigned short SequencerPlaybackEngine::getEventFrameOffset() const
{
    return tickFrameOffset;
}

void SequencerPlaybackEngine::stop()
{
    if (!sequencerIsRunning.load())
    {
        return;
    }

    sequencerIsRunning.store(false);
    tickFrameOffset = 0;
}

bool SequencerPlaybackEngine::isRunning() const
{
    return sequencerIsRunning.load();
}

void SequencerPlaybackEngine::setTickPositionEffectiveImmediately(
    const int newTickPos) const
{
    sequencer->getTransport()->setPosition(
        Sequencer::ticksToQuarterNotes(newTickPos));
    sequencer->getStateManager()->drainQueue();
}

std::shared_ptr<Sequence> SequencerPlaybackEngine::switchToNextSequence() const
{
    sequencer->playToTick(sequencer->getTransport()->getTickPosition());
    sequencer->setActiveSequenceIndex(sequencer->getNextSq(), false);
    sequencer->setNextSq(-1);
    setTickPositionEffectiveImmediately(0);
    auto newSeq = sequencer->getCurrentlyPlayingSequence();
    newSeq->initLoop();
    return newSeq;
}

void SequencerPlaybackEngine::triggerClickIfNeeded() const
{
    if (!sequencer->getTransport()->isCountEnabled())
    {
        return;
    }

    const bool isStepEditor =
        layeredScreen->isCurrentScreen({ScreenId::StepEditorScreen});

    const auto currentScreenName = layeredScreen->getCurrentScreenName();

    const auto countMetronomeScreen =
        getScreens()->get<ScreenId::CountMetronomeScreen>();

    if (sequencer->getTransport()->isRecordingOrOverdubbing())
    {
        if (!countMetronomeScreen->getInRec() &&
            !sequencer->getTransport()->isCountingIn())
        {
            return;
        }
    }
    else
    {

        if (!isStepEditor && !countMetronomeScreen->getInPlay() &&
            !sequencer->getTransport()->isCountingIn() &&
            !isRecMainWithoutPlaying())
        {
            return;
        }
    }

    const auto pos = metronomeOnly
                         ? metronomeOnlyTickPosition
                         : sequencer->getTransport()->getTickPosition();
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

void SequencerPlaybackEngine::stopCountingInIfRequired() const
{
    if (sequencer->getTransport()->getTickPosition() >=
        sequencer->getTransport()->getCountInEndPosTicks())
    {
        setTickPositionEffectiveImmediately(
            sequencer->getTransport()->getCountInStartPosTicks());
        sequencer->getTransport()->setCountingIn(false);
        sequencer->getTransport()->resetCountInPositions();
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

    sequencer->playToTick(seq->getLastTick() - 1);
    sequencer->getTransport()->incrementPlayedStepRepetitions();
    const auto songScreen = getScreens()->get<ScreenId::SongScreen>();
    const auto song = sequencer->getSong(songScreen->getActiveSongIndex());
    const auto step = songScreen->getOffset() + 1;

    const auto doneRepeating =
        sequencer->getTransport()->getPlayedStepRepetitions() >=
        song->getStep(step).lock()->getRepeats();
    const auto reachedLastStep = step == song->getStepCount() - 1;

    if (doneRepeating && song->isLoopEnabled() && step == song->getLastStep())
    {
        sequencer->getTransport()->resetPlayedStepRepetitions();
        songScreen->setOffset(song->getFirstStep() - 1);

        if (const auto newStep =
                song->getStep(songScreen->getOffset() + 1).lock();
            !sequencer->getSequence(newStep->getSequence())->isUsed())
        {
            stopSequencer();
            return true;
        }

        setTickPositionEffectiveImmediately(0);
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
            songScreen->setOffset(songScreen->getOffset() + 1);

            const auto newStep =
                song->getStep(songScreen->getOffset() + 1).lock();

            if (!sequencer->getSequence(newStep->getSequence())->isUsed())
            {
                stopSequencer();
                return true;
            }
        }
        else
        {
            sequencer->playToTick(seq->getLastTick() - 1);
        }
        setTickPositionEffectiveImmediately(0);
    }

    return false;
}

bool SequencerPlaybackEngine::processSeqLoopEnabled() const
{
    if (const auto seq = sequencer->getCurrentlyPlayingSequence();
        sequencer->getTransport()->getTickPosition() >= seq->getLoopEnd() - 1)
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

        sequencer->playToTick(sequencer->getTransport()->getTickPosition());
        setTickPositionEffectiveImmediately(seq->getLoopStart());

        if (sequencer->getTransport()->isRecordingOrOverdubbing())
        {
            if (sequencer->getTransport()->isRecording())
            {
                sequencer->getTransport()->switchRecordToOverdub();
            }
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

            seq->insertBars(1, seq->getLastBarIndex());
            seq->setTimeSignature(seq->getLastBarIndex(),
                                  seq->getLastBarIndex(),
                                  userScreen->timeSig.getNumerator(),
                                  userScreen->timeSig.getDenominator());
        }
        else
        {
            sequencer->getTransport()->stop(
                Transport::StopMode::AT_START_OF_TICK);
            setTickPositionEffectiveImmediately(
                Sequencer::ticksToQuarterNotes(seq->getLastTick()));
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
        noteRepeatProcessor->process(
            this, sequencer->getTransport()->getTickPosition(),
            repeatIntervalTicks, getEventFrameOffset(),
            sequencer->getTransport()->getTempo(),
            static_cast<float>(getSampleRate()));
    }
}

void SequencerPlaybackEngine::stopSequencer() const
{
    auto seq = sequencer->getCurrentlyPlayingSequence();
    sequencer->getTransport()->stop();
    setTickPositionEffectiveImmediately(0);
}

void SequencerPlaybackEngine::enqueueEventAfterNFrames(
    const std::function<void()> &event, const unsigned long nFrames) const
{
    EventAfterNFrames e;
    e.f = event;
    e.nFrames = nFrames;
    eventQueue->enqueue(std::move(e));
}

void SequencerPlaybackEngine::processEventsAfterNFrames()
{
    EventAfterNFrames batch[100];

    const size_t count = eventQueue->try_dequeue_bulk(batch, 100);

    tempEventQueue.clear();

    for (size_t i = 0; i < count; ++i)
    {
        if (auto &e = batch[i]; ++e.frameCounter >= e.nFrames)
        {
            e.f();
        }
        else
        {
            tempEventQueue.push_back(std::move(e));
        }
    }

    for (auto &e : tempEventQueue)
    {
        eventQueue->enqueue(std::move(e));
    }
}

void SequencerPlaybackEngine::work(const int nFrames)
{
    const bool sequencerIsRunningAtStartOfBuffer = sequencerIsRunning.load();
    const auto sampleRate = getSampleRate();

    if (sequencerIsRunningAtStartOfBuffer && metronomeOnly)
    {
        clock->processBufferInternal(sequencer->getTransport()->getTempo(),
                                     sampleRate, nFrames, 0);
        const auto &ticksForCurrentBuffer = clock->getTicksForCurrentBuffer();

        for (uint16_t frameIndex = 0; frameIndex < nFrames; frameIndex++)
        {
            processEventsAfterNFrames();

            if (std::find(ticksForCurrentBuffer.begin(),
                          ticksForCurrentBuffer.end(),
                          frameIndex) != ticksForCurrentBuffer.end())
            {
                triggerClickIfNeeded();
                metronomeOnlyTickPosition++;
            }
        }

        clock->clearTicks();

        return;
    }

    if (sequencerIsRunningAtStartOfBuffer && isAudioServerCurrentlyRunningOffline())
    {
        clock->processBufferInternal(
            sequencer->getTransport()->getTempo(),
            sampleRate,
            nFrames,
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
            sequencer->getTransport()->setPositionWithinSong(
                hostPositionQuarterNotes);
        }
        else
        {
            sequencer->getTransport()->setPosition(hostPositionQuarterNotes);
        }
        sequencer->getStateManager()->drainQueue();
    }

    bool songHasStopped = false;
    bool normalPlayHasStopped = false;

    midiClockOutput->processSampleRateChange();
    midiClockOutput->processTempoChange();

    for (int frameIndex = 0; frameIndex < nFrames; frameIndex++)
    {
        midiClockOutput->processFrame(sequencerIsRunningAtStartOfBuffer,
                                      frameIndex);

        processEventsAfterNFrames();

        if (!sequencerIsRunningAtStartOfBuffer)
        {
            continue;
        }

        if (const auto syncScreen = getScreens()->get<ScreenId::SyncScreen>();
            syncScreen->modeOut != 0 && !isBouncing())
        {
            if (midiClockOutput->isLastProcessedFrameMidiClockLock())
            {
                shouldWaitForMidiClockLock = false;
            }
            else if (shouldWaitForMidiClockLock)
            {
                continue;
            }
        }

        size_t tickCountAtThisFrameIndex = 0;

        for (size_t tickIndex = 0; tickIndex < clockTicks.size(); tickIndex++)
        {
            if (const auto tickFrameIndex = clockTicks[tickIndex];
                tickFrameIndex == frameIndex)
            {
                tickCountAtThisFrameIndex++;
            }
        }

        if (tickCountAtThisFrameIndex == 0)
        {
            continue;
        }

        if (tickCountAtThisFrameIndex > 1)
        {
            sequencer->getTransport()->bumpPositionByTicks(
                tickCountAtThisFrameIndex - 1);
            sequencer->getStateManager()->drainQueue();
        }

        tickFrameOffset = frameIndex;

        triggerClickIfNeeded();
        displayPunchRects();

        if (sequencer->getTransport()->isCountingIn())
        {
            sequencer->getTransport()->bumpPositionByTicks(1);
            sequencer->getStateManager()->drainQueue();
            stopCountingInIfRequired();
            continue;
        }

        if (sequencer->getTransport()->getTickPosition() >=
                seq->getLastTick() - 1 &&
            !sequencer->isSongModeEnabled() && sequencer->getNextSq() != -1)
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
            sequencer->playToTick(sequencer->getTransport()->getTickPosition());
            processNoteRepeat();
        }

        sequencer->getTransport()->bumpPositionByTicks(1);
        sequencer->getStateManager()->drainQueue();
    }

    clock->clearTicks();
}

uint64_t SequencerPlaybackEngine::getMetronomeOnlyTickPosition() const
{
    return metronomeOnlyTickPosition;
}
