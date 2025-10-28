#include "FrameSeq.hpp"

#include "Mpc.hpp"

#include "audiomidi/AudioMidiServices.hpp"

#include "controller/ClientEventController.hpp"
#include "controller/ClientHardwareEventController.hpp"
#include "hardware/Hardware.hpp"

#include "sequencer/Song.hpp"
#include "sequencer/Step.hpp"
#include "sequencer/Track.hpp"
#include "sequencer/RepeatPad.hpp"
#include "Clock.hpp"

#include "lcdgui/screens/window/TimingCorrectScreen.hpp"
#include "lcdgui/screens/window/CountMetronomeScreen.hpp"
#include "lcdgui/screens/SongScreen.hpp"
#include "lcdgui/screens/PunchScreen.hpp"
#include "lcdgui/screens/UserScreen.hpp"
#include "lcdgui/screens/SequencerScreen.hpp"

#include "sequencer/SeqUtil.hpp"

#include "engine/audio/server/NonRealTimeAudioServer.hpp"

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;
using namespace mpc::sequencer;
using namespace mpc::engine::midi;

FrameSeq::FrameSeq(mpc::Mpc &mpc)
    : mpc(mpc), sequencer(mpc.getSequencer()),
      countMetronomeScreen(mpc.screens->get<CountMetronomeScreen>()),
      timingCorrectScreen(mpc.screens->get<TimingCorrectScreen>()),
      sequencerScreen(mpc.screens->get<SequencerScreen>()),
      syncScreen(mpc.screens->get<SyncScreen>()),
      punchScreen(mpc.screens->get<PunchScreen>()),
      songScreen(mpc.screens->get<SongScreen>()),
      userScreen(mpc.screens->get<UserScreen>()),
      midiClockOutput(std::make_shared<MidiClockOutput>(mpc))
{
}

void FrameSeq::start(const bool metronomeOnlyToUse)
{
    if (sequencerIsRunning.load())
    {
        return;
    }

    if (syncScreen->modeOut != 0)
    {
        shouldWaitForMidiClockLock = true;
    }

    mpc.getClock()->reset();

    metronomeOnly = metronomeOnlyToUse;
    metronomeOnlyTickPosition = 0;

    sequencerIsRunning.store(true);
}

void FrameSeq::startMetronome()
{
    if (sequencerIsRunning.load())
    {
        return;
    }

    start(true);
}

unsigned short FrameSeq::getEventFrameOffset() const
{
    return tickFrameOffset;
}

void FrameSeq::stop()
{
    if (!sequencerIsRunning.load())
    {
        return;
    }

    sequencerIsRunning.store(false);
    tickFrameOffset = 0;
}

bool FrameSeq::isRunning()
{
    return sequencerIsRunning.load();
}

void FrameSeq::move(int newTickPos)
{
    sequencer->move(Sequencer::ticksToQuarterNotes(newTickPos));
    updateTimeDisplay();
}

std::shared_ptr<Sequence> FrameSeq::switchToNextSequence()
{
    sequencer->playToTick(sequencer->getTickPosition());
    sequencer->setCurrentlyPlayingSequenceIndex(sequencer->getNextSq());
    sequencer->setNextSq(-1);
    sequencer->move(0.0);
    auto newSeq = sequencer->getCurrentlyPlayingSequence();
    newSeq->initLoop();
    move(0);
    return newSeq;
}

void FrameSeq::triggerClickIfNeeded()
{
    if (!sequencer->isCountEnabled())
    {
        return;
    }

    const bool isStepEditor =
        mpc.getLayeredScreen()->isCurrentScreen<StepEditorScreen>();

    const auto currentScreenName =
        mpc.getLayeredScreen()->getCurrentScreenName();

    const bool isRecMainWithoutPlaying = SeqUtil::isRecMainWithoutPlaying(
        sequencer, timingCorrectScreen, currentScreenName,
        mpc.getHardware()->getButton(hardware::ComponentId::REC),
        mpc.clientEventController->clientHardwareEventController);

    if (sequencer->isRecordingOrOverdubbing())
    {
        if (!countMetronomeScreen->getInRec() && !sequencer->isCountingIn())
        {
            return;
        }
    }
    else
    {

        if (!isStepEditor && !countMetronomeScreen->getInPlay() &&
            !sequencer->isCountingIn() && !isRecMainWithoutPlaying)
        {
            return;
        }
    }

    const auto pos = metronomeOnly ? metronomeOnlyTickPosition
                                   : sequencer->getTickPosition();
    const auto bar = sequencer->getCurrentBarIndex();
    const auto seq = sequencer->getCurrentlyPlayingSequence();
    const auto firstTickOfBar = seq->getFirstTickOfBar(bar);
    const auto relativePos = pos - firstTickOfBar;

    if ((isStepEditor || isRecMainWithoutPlaying) && relativePos == 0)
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
    }

    if (relativePos % static_cast<int>(denTicks) == 0)
    {
        mpc.getSampler()->playMetronome(relativePos == 0 ? 127 : 64,
                                        getEventFrameOffset());
    }
}

void FrameSeq::displayPunchRects()
{
    auto punch =
        sequencer->isPunchEnabled() && sequencer->isRecordingOrOverdubbing();

    if (punch)
    {
        bool punchIn = sequencer->getAutoPunchMode() == 0 ||
                       sequencer->getAutoPunchMode() == 2;
        bool punchOut = sequencer->getAutoPunchMode() == 1 ||
                        sequencer->getAutoPunchMode() == 2;
        auto punchInTime = sequencer->getPunchInTime();
        auto punchOutTime = sequencer->getPunchOutTime();

        if (punchIn && sequencer->getTickPosition() == punchInTime)
        {
            sequencerScreen->setPunchRectOn(0, false);
            sequencerScreen->setPunchRectOn(1, true);
        }
        else if (punchOut && sequencer->getTickPosition() == punchOutTime)
        {
            sequencerScreen->setPunchRectOn(1, false);
            sequencerScreen->setPunchRectOn(2, true);
        }
    }
}

void FrameSeq::stopCountingInIfRequired()
{
    if (sequencer->getTickPosition() >= sequencer->countInEndPos)
    {
        move(sequencer->countInStartPos);
        sequencer->setCountingIn(false);
        sequencer->countInStartPos = -1;
        sequencer->countInEndPos = -1;
    }
}

// Returns true if the song has stopped
bool FrameSeq::processSongMode()
{
    auto seq = sequencer->getCurrentlyPlayingSequence();

    if (sequencer->getTickPosition() < seq->getLastTick())
    {
        return false;
    }

    sequencer->playToTick(seq->getLastTick() - 1);
    sequencer->incrementPlayedStepRepetitions();
    auto song = sequencer->getSong(songScreen->getActiveSongIndex());
    auto step = songScreen->getOffset() + 1;

    auto doneRepeating = sequencer->getPlayedStepRepetitions() >=
                         song->getStep(step).lock()->getRepeats();
    auto reachedLastStep = step == song->getStepCount() - 1;

    if (doneRepeating && song->isLoopEnabled() && step == song->getLastStep())
    {
        sequencer->resetPlayedStepRepetitions();
        songScreen->setOffset(song->getFirstStep() - 1);
        auto newStep = song->getStep(songScreen->getOffset() + 1).lock();

        if (!sequencer->getSequence(newStep->getSequence())->isUsed())
        {
            stopSequencer();
            return true;
        }

        move(0);
    }
    else if (doneRepeating && reachedLastStep)
    {
        sequencer->setEndOfSong(true);
        stopSequencer();
        return true;
    }
    else
    {
        if (doneRepeating)
        {
            sequencer->resetPlayedStepRepetitions();
            songScreen->setOffset(songScreen->getOffset() + 1);

            auto newStep = song->getStep(songScreen->getOffset() + 1).lock();

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
        move(0);
    }

    return false;
}

bool FrameSeq::processSeqLoopEnabled()
{
    auto seq = sequencer->getCurrentlyPlayingSequence();

    if (sequencer->getTickPosition() >= seq->getLoopEnd() - 1)
    {
        auto punch = sequencer->isPunchEnabled() &&
                     sequencer->isRecordingOrOverdubbing();
        bool punchIn = sequencer->getAutoPunchMode() == 0 ||
                       sequencer->getAutoPunchMode() == 2;
        bool punchOut = sequencer->getAutoPunchMode() == 1 ||
                        sequencer->getAutoPunchMode() == 2;

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

        sequencer->playToTick(sequencer->getTickPosition());
        move(seq->getLoopStart());

        if (sequencer->isRecordingOrOverdubbing())
        {
            if (sequencer->isRecording())
            {
                sequencer->switchRecordToOverdub();
                mpc.getHardware()
                    ->getLed(hardware::ComponentId::REC_LED)
                    ->setEnabled(false);
                mpc.getHardware()
                    ->getLed(hardware::ComponentId::OVERDUB_LED)
                    ->setEnabled(true);
            }
        }

        return true;
    }

    return false;
}

bool FrameSeq::processSeqLoopDisabled()
{
    auto seq = sequencer->getCurrentlyPlayingSequence();

    if (sequencer->getTickPosition() >= seq->getLastTick())
    {
        if (sequencer->isRecordingOrOverdubbing())
        {
            seq->insertBars(1, seq->getLastBarIndex());
            seq->setTimeSignature(seq->getLastBarIndex(),
                                  seq->getLastBarIndex(),
                                  userScreen->timeSig.getNumerator(),
                                  userScreen->timeSig.getDenominator());
        }
        else
        {
            sequencer->stop(Sequencer::StopMode::AT_START_OF_TICK);
            mpc.getHardware()
                ->getLed(hardware::ComponentId::PLAY_LED)
                ->setEnabled(false);
            sequencer->move(Sequencer::ticksToQuarterNotes(seq->getLastTick()));
        }

        return true;
    }

    return false;
}

void FrameSeq::processNoteRepeat()
{
    const bool isNoteRepeatLockedOrPressed =
        mpc.clientEventController->clientHardwareEventController
            ->isNoteRepeatLocked() ||
        mpc.getHardware()
            ->getButton(hardware::ComponentId::TAP_TEMPO_OR_NOTE_REPEAT)
            ->isPressed();

    if (!isNoteRepeatLockedOrPressed)
    {
        return;
    }

    auto repeatIntervalTicks = timingCorrectScreen->getNoteValueLengthInTicks();
    int swingPercentage = timingCorrectScreen->getSwing();
    int swingOffset = (int)((swingPercentage - 50) * (4.0 * 0.01) *
                            (repeatIntervalTicks * 0.5));
    auto shiftTiming = timingCorrectScreen->getAmount() *
                       (timingCorrectScreen->isShiftTimingLater() ? 1 : -1);
    auto tickPosWithShift = sequencer->getTickPosition() - shiftTiming;

    bool shouldRepeatPad = false;

    if (repeatIntervalTicks == 24 || repeatIntervalTicks == 48)
    {
        if (tickPosWithShift % (repeatIntervalTicks * 2) ==
                swingOffset + repeatIntervalTicks ||
            tickPosWithShift % (repeatIntervalTicks * 2) == 0)
        {
            shouldRepeatPad = true;
        }
    }
    else if (repeatIntervalTicks != 1 &&
             (tickPosWithShift % repeatIntervalTicks == 0))
    {
        shouldRepeatPad = true;
    }

    if (shouldRepeatPad)
    {
        RepeatPad::process(
            mpc, sequencer->getTickPosition(), repeatIntervalTicks,
            getEventFrameOffset(), sequencer->getTempo(),
            static_cast<float>(
                mpc.getAudioMidiServices()->getAudioServer()->getSampleRate()));
    }
}

void FrameSeq::updateTimeDisplay()
{
    if (!sequencer->isCountingIn())
    {
        sequencer->notifyTimeDisplayRealtime();
        sequencer->notifyObservers(std::string("timesignature"));
    }
}

void FrameSeq::processTempoChange()
{
    if (previousTempo != sequencer->getTempo())
    {
        previousTempo = sequencer->getTempo();
        sequencer->notify("tempo");
    }
}

void FrameSeq::stopSequencer()
{
    auto seq = sequencer->getCurrentlyPlayingSequence();
    sequencer->stop();
    mpc.getHardware()
        ->getLed(hardware::ComponentId::REC_LED)
        ->setEnabled(false);
    mpc.getHardware()
        ->getLed(hardware::ComponentId::OVERDUB_LED)
        ->setEnabled(false);
    mpc.getHardware()
        ->getLed(hardware::ComponentId::PLAY_LED)
        ->setEnabled(false);
    move(0);
}

void FrameSeq::enqueueEventAfterNFrames(const std::function<void()> &event,
                                        unsigned long nFrames)
{
    for (auto &e : eventsAfterNFrames)
    {
        if (!e.occupied.load())
        {
            e.init(nFrames, event);
            break;
        }
    }
}

void FrameSeq::setSampleRate(unsigned int sampleRate)
{
    requestedSampleRate = sampleRate;
}

void FrameSeq::processEventsAfterNFrames(int frameIndex)
{
    for (auto &e : eventsAfterNFrames)
    {
        if (!e.occupied.load())
        {
            continue;
        }

        e.frameCounter += 1;

        if (e.frameCounter >= e.nFrames)
        {
            e.f();
            e.reset();
        }
    }
}

void FrameSeq::work(int nFrames)
{
    const auto clock = mpc.getClock();
    const bool sequencerIsRunningAtStartOfBuffer = sequencerIsRunning.load();
    const auto sampleRate =
        mpc.getAudioMidiServices()->getAudioServer()->getSampleRate();

    if (sequencerIsRunningAtStartOfBuffer && metronomeOnly)
    {
        clock->processBufferInternal(sequencer->getTempo(), sampleRate, nFrames,
                                     0);
        const auto &ticksForCurrentBuffer = clock->getTicksForCurrentBuffer();

        for (uint16_t frameIndex = 0; frameIndex < nFrames; frameIndex++)
        {
            if (std::find(ticksForCurrentBuffer.begin(),
                          ticksForCurrentBuffer.end(),
                          frameIndex) != ticksForCurrentBuffer.end())
            {
                triggerClickIfNeeded();
                metronomeOnlyTickPosition++;
            }
        }

        return;
    }

    const bool isBouncing = mpc.getAudioMidiServices()->isBouncing();
    const auto tempo = mpc.getSequencer()->getTempo();

    const auto &clockTicks = clock->getTicksForCurrentBuffer();
    const bool positionalJumpOccurred = clock->didJumpOccurInLastBuffer();

    auto seq = sequencer->getCurrentlyPlayingSequence();

    if (positionalJumpOccurred)
    {
        const auto hostPositionQuarterNotes =
            clock->getLastProcessedHostPositionQuarterNotes();

        if (mpc.getLayeredScreen()->getCurrentScreenName() == "song")
        {
            sequencer->moveWithinSong(hostPositionQuarterNotes);
        }
        else
        {
            sequencer->move(hostPositionQuarterNotes);
        }
    }

    bool songHasStopped = false;
    bool normalPlayHasStopped = false;

    processTempoChange();

    midiClockOutput->processSampleRateChange();
    midiClockOutput->processTempoChange();

    for (size_t tickIndex = 0; tickIndex < clockTicks.size(); tickIndex++)
    {
        if (clockTicks[tickIndex] >= nFrames)
        {
            throw std::exception();
        }
    }

    for (int frameIndex = 0; frameIndex < nFrames; frameIndex++)
    {
        midiClockOutput->processFrame(sequencerIsRunningAtStartOfBuffer,
                                      frameIndex);

        processEventsAfterNFrames(frameIndex);

        if (!sequencerIsRunningAtStartOfBuffer)
        {
            continue;
        }

        if (syncScreen->modeOut != 0 && !isBouncing)
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
            if (auto tickFrameIndex = clockTicks[tickIndex];
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
            sequencer->bumpPositionByTicks(tickCountAtThisFrameIndex - 1);
        }

        tickFrameOffset = frameIndex;

        triggerClickIfNeeded();
        displayPunchRects();

        if (sequencer->isCountingIn())
        {
            sequencer->bumpPositionByTicks(1);
            stopCountingInIfRequired();
            continue;
        }

        updateTimeDisplay();

        if (sequencer->getTickPosition() >= seq->getLastTick() - 1 &&
            !sequencer->isSongModeEnabled() && sequencer->getNextSq() != -1)
        {
            seq = switchToNextSequence();
            continue;
        }
        else if (sequencer->isSongModeEnabled())
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
            sequencer->playToTick(sequencer->getTickPosition());
            processNoteRepeat();
        }

        sequencer->bumpPositionByTicks(1);
    }
}

uint64_t FrameSeq::getMetronomeOnlyTickPosition()
{
    return metronomeOnlyTickPosition;
}
