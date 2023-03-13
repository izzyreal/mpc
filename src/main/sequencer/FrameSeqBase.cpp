#include "FrameSeqBase.hpp"

#include "FrameSeq.hpp"

#include "Mpc.hpp"

#include "audiomidi/EventHandler.hpp"
#include "audiomidi/MpcMidiOutput.hpp"
#include "audiomidi/AudioMidiServices.hpp"

#include "hardware/Hardware.hpp"
#include "hardware/HwPad.hpp"
#include "hardware/TopPanel.hpp"
#include "controls/GlobalReleaseControls.hpp"

#include "sequencer/Song.hpp"
#include "sequencer/Step.hpp"
#include "sequencer/Track.hpp"
#include "sequencer/SeqUtil.hpp"
#include "sequencer/RepeatPad.hpp"


#include "lcdgui/screens/window/TimingCorrectScreen.hpp"
#include "lcdgui/screens/window/CountMetronomeScreen.hpp"
#include "lcdgui/screens/SyncScreen.hpp"
#include "lcdgui/screens/SongScreen.hpp"
#include "lcdgui/screens/PunchScreen.hpp"
#include "lcdgui/screens/UserScreen.hpp"
#include "lcdgui/screens/SequencerScreen.hpp"

#include "mpc/MpcSoundPlayerChannel.hpp"
#include "midi/ShortMessage.hpp"

#include "Util.hpp"

#include "audio/server/NonRealTimeAudioServer.hpp"

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;
using namespace mpc::sequencer;
using namespace ctoot::midi::core;

FrameSeqBase::FrameSeqBase(mpc::Mpc& mpc)
        : mpc(mpc),
          sequencer(mpc.getSequencer()),
          countMetronomeScreen(mpc.screens->get<CountMetronomeScreen>("count-metronome")),
          timingCorrectScreen(mpc.screens->get<TimingCorrectScreen>("timing-correct")),
          sequencerScreen(mpc.screens->get<SequencerScreen>("sequencer")),
          syncScreen(mpc.screens->get<SyncScreen>("sync")),
          punchScreen(mpc.screens->get<PunchScreen>("punch")),
          songScreen(mpc.screens->get<SongScreen>("song")),
          userScreen(mpc.screens->get<UserScreen>("user")),
          midiSyncStartStopContinueMsg(std::make_shared<ShortMessage>()),
          msg(std::make_shared<ShortMessage>())
{
    msg->setMessage(ShortMessage::TIMING_CLOCK);
    clock.init(requestedSampleRate);
}

void FrameSeqBase::start() {
    if (running.load())
    {
        return;
    }

    running.store(true);
}

void FrameSeqBase::startMetronome() {
    if (running.load())
    {
        return;
    }

    metronome = true;
    start();
}

void FrameSeqBase::sendMidiClockMsg()
{
    auto clockMsg = std::make_shared<ctoot::midi::core::ShortMessage>();
    clockMsg->setMessage(ShortMessage::TIMING_CLOCK);

    if (syncScreen->getModeOut() > 0) {
        clockMsg->bufferPos = tickFrameOffset;

        if (syncScreen->getOut() == 0 || syncScreen->getOut() == 2) {
            mpc.getMidiOutput()->enqueueMessageOutputA(clockMsg);
        }

        if (syncScreen->getOut() == 1 || syncScreen->getOut() == 2) {
            mpc.getMidiOutput()->enqueueMessageOutputB(clockMsg);
        }
    }
}

unsigned short FrameSeqBase::getEventFrameOffset() const
{
    return tickFrameOffset;
}

void FrameSeqBase::stop()
{
    if (!running.load())
    {
        return;
    }

    running.store(false);
    sequencerShouldStartPlayingOnNextLock = false;
    tickFrameOffset = 0;
}

bool FrameSeqBase::isRunning()
{
    return running.load();
}

unsigned int FrameSeqBase::getTickPosition() const
{
    return static_cast<unsigned int>(sequencerPlayTickCounter);
}

void FrameSeqBase::move(int newTickPos)
{
    sequencerPlayTickCounter = newTickPos;
    sequencer->move(newTickPos);
    updateTimeDisplay();
}

std::shared_ptr<Sequence> FrameSeqBase::switchToNextSequence()
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

void FrameSeqBase::triggerClickIfNeeded()
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
        NoteEvent clickEvent(1);
        clickEvent.setVelocity(relativePos == 0 ? 127 : 64);
        mpc.getSampler()->playMetronome(&clickEvent, getEventFrameOffset());
    }
}

void FrameSeqBase::sendMidiSyncMsg(unsigned char status, unsigned int frameIndex)
{
    midiSyncStartStopContinueMsg->setMessage(status);

    midiSyncStartStopContinueMsg->bufferPos = static_cast<int>(frameIndex);

    if (syncScreen->getModeOut() > 0)
    {
        midiSyncStartStopContinueMsg->setMessage(status);

        if (syncScreen->getOut() == 0 || syncScreen->getOut() == 2)
        {
            mpc.getMidiOutput()->enqueueMessageOutputA(midiSyncStartStopContinueMsg);
        }

        if (syncScreen->getOut() == 1 || syncScreen->getOut() == 2)
        {
            mpc.getMidiOutput()->enqueueMessageOutputB(midiSyncStartStopContinueMsg);
        }
    }
}

void FrameSeqBase::displayPunchRects()
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

void FrameSeqBase::stopCountingInIfRequired()
{
    if (getTickPosition() >= sequencer->countInEndPos)
    {
        move(sequencer->countInStartPos);
        sequencer->setCountingIn(false);
        sequencer->countInStartPos = -1;
        sequencer->countInEndPos = -1;
    }
}

bool FrameSeqBase::processSongMode()
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
            sequencer->playToTick(seq->getLastTick() - 1);
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
            sequencer->playToTick(seq->getLastTick() - 1);

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
            move(0);
        }
    }

    return false;
}

bool FrameSeqBase::processSeqLoopEnabled()
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

void FrameSeqBase::processSeqLoopDisabled()
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
    }
}

void FrameSeqBase::processNoteRepeat()
{
    auto controls = mpc.getControls();

    if (controls && (controls->isTapPressed() || controls->isNoteRepeatLocked()))
    {
        int repeatIntervalTicks = sequencer->getTickValues()[timingCorrectScreen->getNoteValue()];
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
                    clock.getBpm(),
                    static_cast<float>(clock.getSampleRate()));
        }
    }
}

void FrameSeqBase::updateTimeDisplay()
{
    if (!sequencer->isCountingIn() && !metronome)
    {
        sequencer->notifyTimeDisplayRealtime();
        sequencer->notifyObservers(std::string("timesignature"));
    }
}

void FrameSeqBase::processTempoChange()
{
    double tempo = sequencer->getTempo();

    if (tempo != clock.getBpm())
    {
        clock.set_bpm(tempo);
        sequencer->notify("tempo");
    }
}

void FrameSeqBase::stopSequencer()
{
    auto seq = sequencer->getCurrentlyPlayingSequence();
    sequencer->playToTick(seq->getLastTick() - 1);
    sequencer->stop();
    move(0);
}

void FrameSeqBase::enqueueEventAfterNFrames(const std::function<void(unsigned int)>& event, unsigned long nFrames)
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

void FrameSeqBase::enqueueMidiSyncStart1msBeforeNextClock()
{
    const auto durationToNextClockInFrames =
            static_cast<unsigned int>(SeqUtil::ticksToFrames(4, clock.getBpm(), static_cast<float>(clock.getSampleRate())));

    const auto oneMsInFrames = static_cast<unsigned int>(clock.getSampleRate() / 1000.f);

    const unsigned int numberOfFramesBeforeMidiSyncStart = durationToNextClockInFrames - oneMsInFrames;

    enqueueEventAfterNFrames([&](unsigned int frameIndex){
            sendMidiSyncMsg(sequencer->getPlayStartTick() == 0 ? ShortMessage::START : ShortMessage::CONTINUE, frameIndex);
    }, numberOfFramesBeforeMidiSyncStart);
}

void FrameSeqBase::setSampleRate(unsigned int sampleRate)
{
    requestedSampleRate = sampleRate;
}

void FrameSeqBase::processEventsAfterNFrames(int frameIndex)
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

bool FrameSeqBase::processTransport(bool isRunningAtStartOfBuffer, int frameIndex)
{
    const auto lockedToClock = midiClockTickCounter++ == 0;

    if (midiClockTickCounter == 4)
    {
        midiClockTickCounter = 0;
    }

    bool sequencerShouldPlay = wasRunning;

    if (lockedToClock)
    {
        sendMidiClockMsg();

        if (!wasRunning && isRunningAtStartOfBuffer)
        {
            wasRunning = true;
            enqueueMidiSyncStart1msBeforeNextClock();
            sequencerShouldStartPlayingOnNextLock = true;
            sequencerPlayTickCounter = sequencer->getPlayStartTick();
        }
        else if (wasRunning && !isRunningAtStartOfBuffer)
        {
            sendMidiSyncMsg(ShortMessage::STOP, frameIndex);
            wasRunning = false;
            sequencerShouldPlay = false;
            metronome = false;
        }
        else if (sequencerShouldStartPlayingOnNextLock)
        {
            sequencerShouldPlay = true;
            sequencerShouldStartPlayingOnNextLock = false;
        }
    }

    return sequencerShouldPlay;
}

void FrameSeqBase::processSampleRateChange()
{
    if (clock.getSampleRate() != requestedSampleRate)
    {
        auto bpm = clock.getBpm();
        clock.init(requestedSampleRate);
        clock.set_bpm(bpm);
    }
}
