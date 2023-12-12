#include "FrameSeq.hpp"

#include "Mpc.hpp"

#include "audiomidi/AudioMidiServices.hpp"
#include "audiomidi/EventHandler.hpp"
#include "audiomidi/MidiOutput.hpp"

#include "hardware/Hardware.hpp"
#include "hardware/HwPad.hpp"
#include "hardware/TopPanel.hpp"
#include "controls/GlobalReleaseControls.hpp"

#include "sequencer/Song.hpp"
#include "sequencer/Step.hpp"
#include "sequencer/Track.hpp"
#include "sequencer/RepeatPad.hpp"
#include "ExternalClock.hpp"

#include "lcdgui/screens/window/TimingCorrectScreen.hpp"
#include "lcdgui/screens/window/CountMetronomeScreen.hpp"
#include <lcdgui/screens/window/VmpcDirectToDiskRecorderScreen.hpp>
#include "lcdgui/screens/SongScreen.hpp"
#include "lcdgui/screens/PunchScreen.hpp"
#include "lcdgui/screens/UserScreen.hpp"
#include "lcdgui/screens/SequencerScreen.hpp"

#include "Util.hpp"

#include <engine/audio/server/NonRealTimeAudioServer.hpp>

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;
using namespace mpc::sequencer;
using namespace mpc::engine::midi;

FrameSeq::FrameSeq(mpc::Mpc& mpc)
        : mpc(mpc),
          sequencer(mpc.getSequencer()),
          countMetronomeScreen(mpc.screens->get<CountMetronomeScreen>("count-metronome")),
          timingCorrectScreen(mpc.screens->get<TimingCorrectScreen>("timing-correct")),
          sequencerScreen(mpc.screens->get<SequencerScreen>("sequencer")),
          syncScreen(mpc.screens->get<SyncScreen>("sync")),
          punchScreen(mpc.screens->get<PunchScreen>("punch")),
          songScreen(mpc.screens->get<SongScreen>("song")),
          userScreen(mpc.screens->get<UserScreen>("user")),
          midiClockOutput(std::make_shared<MidiClockOutput>(mpc))
{
}

void FrameSeq::start()
{
    if (sequencerIsRunning.load())
    {
        return;
    }

    processSampleRateChange();

    if (syncScreen->modeOut == 0)
    {
        internalClock.reset();
    }
    else
    {
        shouldWaitForMidiClockLock = true;
    }

    sequencerPlayTickCounter = sequencer->getPlayStartTick();

    sequencerIsRunning.store(true);
}

void FrameSeq::startMetronome() {
    if (sequencerIsRunning.load())
    {
        return;
    }

    metronome = true;
    start();
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

unsigned int FrameSeq::getTickPosition() const
{
    return static_cast<unsigned int>(sequencerPlayTickCounter);
}

void FrameSeq::move(int newTickPos)
{
    sequencerPlayTickCounter = newTickPos;
    sequencer->move(newTickPos);
    updateTimeDisplay();
}

void FrameSeq::setSequencerPlayTickCounter(unsigned long long value)
{
    sequencerPlayTickCounter = value;
}

std::shared_ptr<Sequence> FrameSeq::switchToNextSequence()
{
    sequencer->playToTick(static_cast<int>(sequencerPlayTickCounter));
    sequencer->setCurrentlyPlayingSequenceIndex(sequencer->getNextSq());
    sequencer->setNextSq(-1);
    sequencer->move(0);
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

    const bool isStepEditor = mpc.getLayeredScreen()->getCurrentScreenName() == "step-editor";

    if (sequencer->isRecordingOrOverdubbing())
    {
        if (!countMetronomeScreen->getInRec() && !sequencer->isCountingIn())
        {
            return;
        }
    }
    else
    {
        if (!isStepEditor && !countMetronomeScreen->getInPlay() && !sequencer->isCountingIn())
        {
            return;
        }
    }

    auto pos = sequencerPlayTickCounter;
    auto bar = sequencer->getCurrentBarIndex();
    auto seq = sequencer->getCurrentlyPlayingSequence();
    auto firstTickOfBar = seq->getFirstTickOfBar(bar);
    auto relativePos = pos - firstTickOfBar;

    if (isStepEditor && relativePos == 0)
    {
        return;
    }

    auto den = seq->getDenominator(bar);
    auto denTicks = 96 * (4.0 / den);

    switch (countMetronomeScreen->getRate())
    {
        case 1:
            denTicks *= 2.0f / 3.f; break;
        case 2:
            denTicks *= 1.0f / 2; break;
        case 3:
            denTicks *= 1.0f / 3; break;
        case 4:
            denTicks *= 1.0f / 4; break;
        case 5:
            denTicks *= 1.0f / 6; break;
        case 6:
            denTicks *= 1.0f / 8; break;
        case 7:
            denTicks *= 1.0f / 12; break;
    }

    if (relativePos % static_cast<int>(denTicks) == 0)
    {
        mpc.getSampler()->playMetronome(relativePos == 0 ? 127 : 64, getEventFrameOffset());
    }
}

void FrameSeq::displayPunchRects()
{
    auto punch = punchScreen->on && sequencer->isRecordingOrOverdubbing();

    if (punch)
    {
        bool punchIn = punchScreen->autoPunch == 0 || punchScreen->autoPunch == 2;
        bool punchOut = punchScreen->autoPunch == 1 || punchScreen->autoPunch == 2;
        auto punchInTime = punchScreen->time0;
        auto punchOutTime = punchScreen->time1;

        if (punchIn && getTickPosition() == punchInTime) {
            sequencerScreen->setPunchRectOn(0, false);
            sequencerScreen->setPunchRectOn(1, true);
        }
        else if (punchOut && getTickPosition() == punchOutTime)
        {
            sequencerScreen->setPunchRectOn(1, false);
            sequencerScreen->setPunchRectOn(2, true);
        }
    }
}

void FrameSeq::stopCountingInIfRequired()
{
    if (getTickPosition() >= sequencer->countInEndPos)
    {
        move(sequencer->countInStartPos);
        sequencer->setCountingIn(false);
        sequencer->countInStartPos = -1;
        sequencer->countInEndPos = -1;
    }
}

bool FrameSeq::processSongMode()
{
    auto seq = sequencer->getCurrentlyPlayingSequence();

    if (getTickPosition() >= seq->getLastTick() - 1)
    {
        sequencer->playToTick(seq->getLastTick() - 1);
        sequencer->incrementPlayedStepRepetitions();
        auto song = sequencer->getSong(songScreen->getActiveSongIndex());
        auto step = songScreen->getOffset() + 1;

        auto doneRepeating = sequencer->getPlayedStepRepetitions() >= song->getStep(step).lock()->getRepeats();
        auto reachedLastStep = step == song->getStepCount() - 1;

        if (doneRepeating && songScreen->isLoopEnabled() && step == song->getLastStep())
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
    }

    return false;
}

bool FrameSeq::processSeqLoopEnabled()
{
    auto seq = sequencer->getCurrentlyPlayingSequence();

    if (sequencerPlayTickCounter >= seq->getLoopEnd() - 1)
    {
        auto punch = punchScreen->on && sequencer->isRecordingOrOverdubbing();
        bool punchIn = punchScreen->autoPunch == 0 || punchScreen->autoPunch == 2;
        bool punchOut = punchScreen->autoPunch == 1 || punchScreen->autoPunch == 2;

        if (punch && punchIn)
        {
            sequencerScreen->setPunchRectOn(0, true);
            sequencerScreen->setPunchRectOn(1, false);
        }

        if (punch && punchOut)
            sequencerScreen->setPunchRectOn(2, false);

        if (punch && punchOut && !punchIn)
            sequencerScreen->setPunchRectOn(1, true);

        sequencer->playToTick(static_cast<int>(sequencerPlayTickCounter));
        move(seq->getLoopStart());

        if (sequencer->isRecordingOrOverdubbing())
        {
            if (sequencer->isRecording())
                sequencer->switchRecordToOverDub();
        }

        return true;
    }

    return false;
}

bool FrameSeq::processSeqLoopDisabled()
{
    auto seq = sequencer->getCurrentlyPlayingSequence();

    if (getTickPosition() >= seq->getLastTick())
    {
        if (sequencer->isRecordingOrOverdubbing())
        {
            seq->insertBars(1, seq->getLastBarIndex());
            seq->setTimeSignature(seq->getLastBarIndex(), seq->getLastBarIndex(), userScreen->timeSig.getNumerator(), userScreen->timeSig.getDenominator());
        }
        else
        {
            sequencer->stop(seq->getLastTick());
            sequencer->move(seq->getLastTick());
        }

        return true;
    }

    return false;
}

void FrameSeq::processNoteRepeat()
{
    auto controls = mpc.getControls();

    if (controls && (controls->isTapPressed() || controls->isNoteRepeatLocked()))
    {
        auto repeatIntervalTicks = timingCorrectScreen->getNoteValueLengthInTicks();
        int swingPercentage = timingCorrectScreen->getSwing();
        int swingOffset = (int)((swingPercentage - 50) * (4.0 * 0.01) * (repeatIntervalTicks * 0.5));
        auto shiftTiming = timingCorrectScreen->getAmount() * (timingCorrectScreen->isShiftTimingLater() ? 1 : -1);
        auto tickPosWithShift = getTickPosition() - shiftTiming;

        bool shouldRepeatPad = false;

        if (repeatIntervalTicks == 24 || repeatIntervalTicks == 48)
        {
            if (tickPosWithShift % (repeatIntervalTicks * 2) == swingOffset + repeatIntervalTicks ||
                tickPosWithShift % (repeatIntervalTicks * 2) == 0)
            {
                shouldRepeatPad = true;
            }
        }
        else if (repeatIntervalTicks != 1 && (tickPosWithShift % repeatIntervalTicks == 0))
        {
            shouldRepeatPad = true;
        }

        if (shouldRepeatPad)
        {
            RepeatPad::process(
                    mpc,
                    getTickPosition(),
                    repeatIntervalTicks,
                    getEventFrameOffset(),
                    internalClock.getBpm(),
                    static_cast<float>(internalClock.getSampleRate()));
        }
    }
}

void FrameSeq::updateTimeDisplay()
{
    if (!sequencer->isCountingIn() && !metronome)
    {
        sequencer->notifyTimeDisplayRealtime();
        sequencer->notifyObservers(std::string("timesignature"));
    }
}

void FrameSeq::processTempoChange()
{
    double tempo = sequencer->getTempo();

    if (tempo != internalClock.getBpm())
    {
        internalClock.set_bpm(tempo);
        sequencer->notify("tempo");
    }
}

void FrameSeq::stopSequencer()
{
    auto seq = sequencer->getCurrentlyPlayingSequence();
    sequencer->stop();
    move(0);
}

void FrameSeq::enqueueEventAfterNFrames(const std::function<void(unsigned int)>& event, unsigned long nFrames)
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
    for (auto& e : eventsAfterNFrames)
    {
        if (!e.occupied.load()) continue;

        e.frameCounter += 1;

        if (e.frameCounter >= e.nFrames)
        {
            e.f(frameIndex);
            e.reset();
        }
    }
}

void FrameSeq::processSampleRateChange()
{
    if (internalClock.getSampleRate() != requestedSampleRate)
    {
        auto bpm = internalClock.getBpm();
        internalClock.init(requestedSampleRate);
        internalClock.set_bpm(bpm);
    }
}

void FrameSeq::work(int nFrames)
{
    const bool sequencerIsRunningAtStartOfBuffer = sequencerIsRunning.load();
    const bool useInternalClock = syncScreen->modeIn == 0;

    auto seq = sequencer->getCurrentlyPlayingSequence();

    bool songHasStopped = false;
    bool normalPlayHasStopped = false;

    processSampleRateChange();
    processTempoChange();

//    midiClockOutput->processSampleRateChange();
//    midiClockOutput->processTempoChange();

    auto& externalClockTicks = mpc.getExternalClock()->getTicksForCurrentBuffer();

    for (auto& externalClockTick : externalClockTicks)
    {
        if (static_cast<int>(externalClockTick) >= nFrames)
        {
            throw std::exception();
        }
    }

    for (int frameIndex = 0; frameIndex < nFrames; frameIndex++)
    {
//        midiClockOutput->processFrame(sequencerIsRunningAtStartOfBuffer, frameIndex);

        processEventsAfterNFrames(frameIndex);

        if (!sequencerIsRunningAtStartOfBuffer)
        {
            continue;
        }

        if (syncScreen->modeOut != 0)
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

        if (useInternalClock)
        {
            if (!internalClock.proc())
            {
                continue;
            }
        }
        else
        {
            const auto frameIndexIsExternalClockTick =
                    std::find_if(externalClockTicks.begin(), externalClockTicks.end(), [&](const double& t){ return static_cast<int>(t) == frameIndex; }) != externalClockTicks.end();

            if (!frameIndexIsExternalClockTick)
            {
                continue;
            }
        }

//        MLOG("tick " + std::to_string(sequencerPlayTickCounter) + " at frame " + std::to_string(frameIndex));

        tickFrameOffset = frameIndex;

        triggerClickIfNeeded();
        displayPunchRects();

        if (metronome)
        {
            sequencerPlayTickCounter++;
            continue;
        }

        if (sequencer->isCountingIn())
        {
            sequencerPlayTickCounter++;
            stopCountingInIfRequired();
            continue;
        }

        updateTimeDisplay();

        if (sequencerPlayTickCounter >= seq->getLastTick() - 1 &&
            !sequencer->isSongModeEnabled() &&
            sequencer->getNextSq() != -1)
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
            sequencer->playToTick(static_cast<int>(sequencerPlayTickCounter));
            processNoteRepeat();
        }

        sequencerPlayTickCounter++;
    }
}
