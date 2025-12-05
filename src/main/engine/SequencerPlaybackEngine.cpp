#include "engine/SequencerPlaybackEngine.hpp"

#include "EngineHost.hpp"
#include "Logger.hpp"
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
#include "lcdgui/screens/UserScreen.hpp"
#include "lcdgui/screens/SequencerScreen.hpp"

#include "sequencer/MidiClockOutput.hpp"

using namespace mpc::engine;
using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;
using namespace mpc::sequencer;

SequencerPlaybackEngine::SequencerPlaybackEngine(
    EngineHost *engineHost,
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
    : engineHost(engineHost), layeredScreen(layeredScreen),
      getScreens(getScreens), sequencer(sequencer), clock(clock),
      isBouncing(isBouncing), getSampleRate(getSampleRate),
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
}

unsigned short SequencerPlaybackEngine::getEventFrameOffset() const
{
    return tickFrameOffset;
}

void SequencerPlaybackEngine::setTickPositionEffectiveImmediately(
    const int newTickPos) const
{
    sequencer->getTransport()->setPosition(
        Sequencer::ticksToQuarterNotes(newTickPos));

    const auto manager = sequencer->getStateManager();
    manager->drainQueue();
    manager->enqueue(SyncTrackEventIndices{});
    manager->drainQueue();
}

std::shared_ptr<Sequence> SequencerPlaybackEngine::switchToNextSequence() const
{
    sequencer->playTick(sequencer->getTransport()->getTickPosition());
    sequencer->setSelectedSequenceIndex(sequencer->getNextSq(), false);
    sequencer->getStateManager()->drainQueue();
    sequencer->setNextSq(NoSequenceIndex);
    setTickPositionEffectiveImmediately(0);
    auto newSeq = sequencer->getCurrentlyPlayingSequence();
    newSeq->syncTrackEventIndices(0);
    return newSeq;
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

void SequencerPlaybackEngine::stopCountingInIfRequired() const
{
    const auto transport = sequencer->getTransport();

    if (transport->getTickPosition() >= transport->getCountInEndPosTicks())
    {
        setTickPositionEffectiveImmediately(
            transport->getCountInStartPosTicks());
        transport->setCountingIn(false);
        sequencer->getStateManager()->drainQueue();
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
    sequencer->getStateManager()->drainQueue();
    const auto song = sequencer->getSelectedSong();
    const auto stepIndex = sequencer->getSelectedSongStepIndex();

    const auto doneRepeating =
        sequencer->getTransport()->getPlayedStepRepetitions() >=
        song->getStep(stepIndex).lock()->getRepeats();

    const auto reachedLastStep = stepIndex == song->getStepCount() - 1;

    if (doneRepeating && song->isLoopEnabled() &&
        stepIndex == song->getLastLoopStepIndex())
    {
        sequencer->getTransport()->resetPlayedStepRepetitions();
        sequencer->setSelectedSongStepIndex(song->getFirstLoopStepIndex());
        sequencer->getStateManager()->drainQueue();

        if (const auto newStep =
                song->getStep(song->getFirstLoopStepIndex()).lock();
            !sequencer->getSequence(newStep->getSequenceIndex())->isUsed())
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
            sequencer->setSelectedSongStepIndex(stepIndex + 1);

            const auto newStep = song->getStep(stepIndex + 1).lock();

            if (!sequencer->getSequence(newStep->getSequenceIndex())->isUsed())
            {
                stopSequencer();
                return true;
            }
        }
        else
        {
            sequencer->playTick(seq->getLastTick() - 1);
        }
        setTickPositionEffectiveImmediately(0);
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
        setTickPositionEffectiveImmediately(seq->getLoopStartTick());

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
            engineHost, sequencer->getTransport()->getTickPosition(),
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

void SequencerPlaybackEngine::work(const int nFrames)
{
    const auto manager = sequencer->getStateManager();
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

        for (uint16_t frameIndex = 0; frameIndex < nFrames; frameIndex++)
        {
            if (std::find(ticksForCurrentBuffer.begin(),
                          ticksForCurrentBuffer.end(),
                          frameIndex) != ticksForCurrentBuffer.end())
            {
                triggerClickIfNeeded();
                manager->enqueue(BumpMetronomeOnlyTickPositionOneTick{});
                manager->drainQueue();
            }
        }

        clock->clearTicks();

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

        PositionQuarterNotes wrappedPosition;

        if (sequencer->isSongModeEnabled())
        {
            wrappedPosition =
                sequencer->getTransport()->getWrappedPositionInSong(
                    hostPositionQuarterNotes);
        }
        else
        {
            wrappedPosition =
                sequencer->getTransport()->getWrappedPositionInSequence(
                    hostPositionQuarterNotes);
        }

        engineHost->flushNoteOffs();
        sequencer->getTransport()->setPosition(wrappedPosition);
        seq->syncTrackEventIndices(
            Sequencer::quarterNotesToTicks(wrappedPosition));
        manager->drainQueue();
    }

    bool songHasStopped = false;
    bool normalPlayHasStopped = false;

    midiClockOutput->processSampleRateChange();
    midiClockOutput->processTempoChange();

    for (int frameIndex = 0; frameIndex < nFrames; frameIndex++)
    {
        size_t tickCountAtThisFrameIndex = 0;

        for (size_t tickIndex = 0; tickIndex < clockTicks.size(); tickIndex++)
        {
            if (const auto tickFrameIndex = clockTicks[tickIndex];
                tickFrameIndex == frameIndex)
            {
                tickCountAtThisFrameIndex++;
            }
        }

        midiClockOutput->processFrame(sequencerIsRunningAtStartOfBuffer,
                                      frameIndex, tickCountAtThisFrameIndex);

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
                MLOG("Waiting for MIDI clock lock");
                continue;
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
            manager->drainQueue();
        }

        tickFrameOffset = frameIndex;

        triggerClickIfNeeded();

        concurrency::Task uiTask;

        uiTask.set(
            [this]
            {
                displayPunchRects();
            });

        layeredScreen->postToUiThread(uiTask);

        if (sequencer->getTransport()->isCountingIn())
        {
            sequencer->getTransport()->bumpPositionByTicks(1);
            manager->drainQueue();
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
            sequencer->playTick(sequencer->getTransport()->getTickPosition());
            processNoteRepeat();
            sequencer->getTransport()->bumpPositionByTicks(1);
        }

        manager->drainQueue();
    }

    clock->clearTicks();
}
