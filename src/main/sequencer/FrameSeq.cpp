#include "sequencer/FrameSeq.hpp"
#include "eventregistry/EventRegistry.hpp"

#include "lcdgui/LayeredScreen.hpp"
#include "lcdgui/screens/SyncScreen.hpp"
#include "sequencer/Sequence.hpp"
#include "sequencer/SequencerStateManager.hpp"
#include "sequencer/Song.hpp"
#include "sequencer/Step.hpp"
#include "sequencer/NoteRepeatProcessor.hpp"
#include "Clock.hpp"

#include "lcdgui/screens/window/TimingCorrectScreen.hpp"
#include "lcdgui/screens/window/CountMetronomeScreen.hpp"
#include "lcdgui/screens/SongScreen.hpp"
#include "lcdgui/screens/UserScreen.hpp"
#include "lcdgui/screens/SequencerScreen.hpp"
#include "lcdgui/screens/MixerSetupScreen.hpp"
#include "lcdgui/screens/window/Assign16LevelsScreen.hpp"

#include "MidiClockOutput.hpp"

#include <concurrentqueue.h>

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;
using namespace mpc::sequencer;
using namespace mpc::eventregistry;

using namespace mpc::sampler;
using namespace mpc::engine;
using namespace mpc::engine::audio::mixer;
using namespace mpc::hardware;

FrameSeq::FrameSeq(
    const std::shared_ptr<EventRegistry> &eventRegistry, Sequencer *sequencer,
    const std::shared_ptr<Clock> &clock,
    const std::shared_ptr<LayeredScreen> &layeredScreen,
    std::function<bool()> isBouncing, const std::function<int()> &getSampleRate,
    const std::function<bool()> &isRecMainWithoutPlaying,
    const std::function<void(int velo, int frameOffset)> &triggerMetronome,
    std::function<std::shared_ptr<Screens>()> getScreens,
    const std::function<bool()> &isNoteRepeatLockedOrPressed,
    // Only used by note repeat (NoteRepeatProcessor)
    const std::shared_ptr<Sampler> &sampler,
    const std::function<std::shared_ptr<AudioMixer>()> &getAudioMixer,
    const std::function<bool()> &isFullLevelEnabled,
    const std::function<bool()> &isSixteenLevelsEnabled,
    const std::shared_ptr<hardware::Slider> &hardwareSlider,
    std::vector<std::shared_ptr<engine::Voice>> *voices,
    const std::function<std::vector<MixerInterconnection *> &()>
        &getMixerInterconnections)
    : eventRegistry(eventRegistry), layeredScreen(layeredScreen),
      getScreens(getScreens), sequencer(sequencer), clock(clock),
      isBouncing(isBouncing), getSampleRate(getSampleRate),
      isRecMainWithoutPlaying(isRecMainWithoutPlaying),
      triggerMetronome(triggerMetronome),
      isNoteRepeatLockedOrPressed(isNoteRepeatLockedOrPressed),
      sampler(sampler), getAudioMixer(getAudioMixer),
      isFullLevelEnabled(isFullLevelEnabled),
      isSixteenLevelsEnabled(isSixteenLevelsEnabled),
      hardwareSlider(hardwareSlider), voices(voices),
      getMixerInterconnections(getMixerInterconnections),
      midiClockOutput(
          std::make_shared<MidiClockOutput>(sequencer, getScreens, isBouncing))
{
    eventQueue = std::make_shared<EventQueue>(100);
    tempEventQueue.reserve(100);
}

void FrameSeq::start(const bool metronomeOnlyToUse)
{
    if (sequencerIsRunning.load())
    {
        return;
    }

    if (getScreens()->get<ScreenId::SyncScreen>()->modeOut != 0)
    {
        shouldWaitForMidiClockLock = true;
    }

    clock->reset();

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

bool FrameSeq::isRunning() const
{
    return sequencerIsRunning.load();
}

void FrameSeq::move(const int newTickPos) const
{
    sequencer->move(Sequencer::ticksToQuarterNotes(newTickPos));
    sequencer->getStateManager()->drainQueue();
}

std::shared_ptr<Sequence> FrameSeq::switchToNextSequence() const
{
    sequencer->playToTick(sequencer->getTickPosition());
    sequencer->setCurrentlyPlayingSequenceIndex(sequencer->getNextSq());
    sequencer->setNextSq(-1);
    move(0);
    auto newSeq = sequencer->getCurrentlyPlayingSequence();
    newSeq->initLoop();
    move(0);
    return newSeq;
}

void FrameSeq::triggerClickIfNeeded() const
{
    if (!sequencer->isCountEnabled())
    {
        return;
    }

    const bool isStepEditor =
        layeredScreen->isCurrentScreen({ScreenId::StepEditorScreen});

    const auto currentScreenName = layeredScreen->getCurrentScreenName();

    const auto countMetronomeScreen =
        getScreens()->get<ScreenId::CountMetronomeScreen>();

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
            !sequencer->isCountingIn() && !isRecMainWithoutPlaying())
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
        triggerMetronome(relativePos == 0 ? 127 : 64, getEventFrameOffset());
    }
}

void FrameSeq::displayPunchRects() const
{
    const auto punch =
        sequencer->isPunchEnabled() && sequencer->isRecordingOrOverdubbing();

    if (punch)
    {
        const bool punchIn = sequencer->getAutoPunchMode() == 0 ||
                             sequencer->getAutoPunchMode() == 2;
        const bool punchOut = sequencer->getAutoPunchMode() == 1 ||
                              sequencer->getAutoPunchMode() == 2;
        const auto punchInTime = sequencer->getPunchInTime();
        const auto punchOutTime = sequencer->getPunchOutTime();

        const auto sequencerScreen =
            getScreens()->get<ScreenId::SequencerScreen>();

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

void FrameSeq::stopCountingInIfRequired() const
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
bool FrameSeq::processSongMode() const
{
    const auto seq = sequencer->getCurrentlyPlayingSequence();

    if (sequencer->getTickPosition() < seq->getLastTick())
    {
        return false;
    }

    sequencer->playToTick(seq->getLastTick() - 1);
    sequencer->incrementPlayedStepRepetitions();
    const auto songScreen = getScreens()->get<ScreenId::SongScreen>();
    const auto song = sequencer->getSong(songScreen->getActiveSongIndex());
    const auto step = songScreen->getOffset() + 1;

    const auto doneRepeating = sequencer->getPlayedStepRepetitions() >=
                               song->getStep(step).lock()->getRepeats();
    const auto reachedLastStep = step == song->getStepCount() - 1;

    if (doneRepeating && song->isLoopEnabled() && step == song->getLastStep())
    {
        sequencer->resetPlayedStepRepetitions();
        songScreen->setOffset(song->getFirstStep() - 1);
        const auto newStep = song->getStep(songScreen->getOffset() + 1).lock();

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
        move(0);
    }

    return false;
}

bool FrameSeq::processSeqLoopEnabled() const
{
    const auto seq = sequencer->getCurrentlyPlayingSequence();

    if (sequencer->getTickPosition() >= seq->getLoopEnd() - 1)
    {
        const auto punch = sequencer->isPunchEnabled() &&
                           sequencer->isRecordingOrOverdubbing();
        const bool punchIn = sequencer->getAutoPunchMode() == 0 ||
                             sequencer->getAutoPunchMode() == 2;
        const bool punchOut = sequencer->getAutoPunchMode() == 1 ||
                              sequencer->getAutoPunchMode() == 2;

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

        sequencer->playToTick(sequencer->getTickPosition());
        move(seq->getLoopStart());

        if (sequencer->isRecordingOrOverdubbing())
        {
            if (sequencer->isRecording())
            {
                sequencer->switchRecordToOverdub();
            }
        }

        return true;
    }

    return false;
}

bool FrameSeq::processSeqLoopDisabled() const
{
    const auto seq = sequencer->getCurrentlyPlayingSequence();

    if (sequencer->getTickPosition() >= seq->getLastTick())
    {
        if (sequencer->isRecordingOrOverdubbing())
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
            sequencer->stop(Sequencer::StopMode::AT_START_OF_TICK);
            move(Sequencer::ticksToQuarterNotes(seq->getLastTick()));
        }

        return true;
    }

    return false;
}

void FrameSeq::processNoteRepeat()
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
    const auto tickPosWithShift = sequencer->getTickPosition() - shiftTiming;

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
             (tickPosWithShift % repeatIntervalTicks == 0))
    {
        shouldRepeatNote = true;
    }

    if (shouldRepeatNote)
    {
        const auto assign16LevelsScreen =
            getScreens()->get<ScreenId::Assign16LevelsScreen>();
        const auto mixerSetupScreen =
            getScreens()->get<ScreenId::MixerSetupScreen>();

        NoteRepeatProcessor::process(
            this, sequencer, sampler, getAudioMixer(), isFullLevelEnabled(),
            isSixteenLevelsEnabled(), assign16LevelsScreen, mixerSetupScreen,
            eventRegistry, hardwareSlider, voices, getMixerInterconnections(),
            sequencer->getTickPosition(), repeatIntervalTicks,
            getEventFrameOffset(), sequencer->getTempo(),
            static_cast<float>(getSampleRate()));
    }
}

void FrameSeq::stopSequencer() const
{
    auto seq = sequencer->getCurrentlyPlayingSequence();
    sequencer->stop();
    move(0);
}

void FrameSeq::enqueueEventAfterNFrames(const std::function<void()> &event,
                                        const unsigned long nFrames) const
{
    EventAfterNFrames e;
    e.f = event;
    e.nFrames = nFrames;
    eventQueue->enqueue(std::move(e));
}

void FrameSeq::processEventsAfterNFrames()
{
    EventAfterNFrames batch[100];

    const size_t count = eventQueue->try_dequeue_bulk(batch, 100);

    tempEventQueue.clear();

    for (size_t i = 0; i < count; ++i)
    {
        auto &e = batch[i];
        if (++e.frameCounter >= e.nFrames)
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

void FrameSeq::work(const int nFrames)
{
    sequencer->getStateManager()->drainQueue();
    eventRegistry->drainQueue();

    const bool sequencerIsRunningAtStartOfBuffer = sequencerIsRunning.load();
    const auto sampleRate = getSampleRate();

    if (sequencerIsRunningAtStartOfBuffer && metronomeOnly)
    {
        clock->processBufferInternal(sequencer->getTempo(), sampleRate, nFrames,
                                     0);
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

        return;
    }

    const auto &clockTicks = clock->getTicksForCurrentBuffer();
    const bool positionalJumpOccurred = clock->didJumpOccurInLastBuffer();

    auto seq = sequencer->getCurrentlyPlayingSequence();

    if (positionalJumpOccurred)
    {
        const auto hostPositionQuarterNotes =
            clock->getLastProcessedHostPositionQuarterNotes();

        if (layeredScreen->getCurrentScreenName() == "song")
        {
            sequencer->moveWithinSong(hostPositionQuarterNotes);
        }
        else
        {
            sequencer->move(hostPositionQuarterNotes);
        }
        sequencer->getStateManager()->drainQueue();
    }

    bool songHasStopped = false;
    bool normalPlayHasStopped = false;

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

        processEventsAfterNFrames();

        if (!sequencerIsRunningAtStartOfBuffer)
        {
            continue;
        }

        const auto syncScreen = getScreens()->get<ScreenId::SyncScreen>();

        if (syncScreen->modeOut != 0 && !isBouncing())
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
            sequencer->bumpPositionByTicks(tickCountAtThisFrameIndex - 1);
            sequencer->getStateManager()->drainQueue();
        }

        tickFrameOffset = frameIndex;

        triggerClickIfNeeded();
        displayPunchRects();

        if (sequencer->isCountingIn())
        {
            sequencer->bumpPositionByTicks(1);
            sequencer->getStateManager()->drainQueue();
            stopCountingInIfRequired();
            continue;
        }

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
        sequencer->getStateManager()->drainQueue();
    }
}

uint64_t FrameSeq::getMetronomeOnlyTickPosition() const
{
    return metronomeOnlyTickPosition;
}
