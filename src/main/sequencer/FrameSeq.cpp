#include "FrameSeq.hpp"

#include <Mpc.hpp>

#include "audiomidi/EventHandler.hpp"
#include "audiomidi/MpcMidiOutput.hpp"

#include <hardware/Hardware.hpp>
#include <hardware/HwPad.hpp>
#include <hardware/TopPanel.hpp>
#include <controls/GlobalReleaseControls.hpp>

#include <sequencer/Song.hpp>
#include <sequencer/Step.hpp>
#include <sequencer/Track.hpp>
#include <sequencer/SeqUtil.hpp>

#include <lcdgui/screens/window/TimingCorrectScreen.hpp>
#include <lcdgui/screens/window/CountMetronomeScreen.hpp>
#include <lcdgui/screens/SyncScreen.hpp>
#include <lcdgui/screens/SongScreen.hpp>
#include <lcdgui/screens/PunchScreen.hpp>
#include <lcdgui/screens/UserScreen.hpp>
#include <lcdgui/screens/SequencerScreen.hpp>

#include "mpc/MpcSoundPlayerChannel.hpp"
#include "mpc/MpcMultiMidiSynth.hpp"
#include "midi/core/ShortMessage.hpp"

#include "Util.hpp"

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;
using namespace mpc::sequencer;
using namespace ctoot::midi::core;

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
      midiSyncClockMsg(std::make_shared<ShortMessage>()),
      midiSyncStartStopContinueMsg(std::make_shared<ShortMessage>())
{
    midiSyncClockMsg->setMessage(ShortMessage::TIMING_CLOCK);
}

void FrameSeq::start(float sampleRate) {
	//MLOG("frameSeq starting with sampleRate " + std::to_string(sampleRate));

	if (running)
		return;

	clock.init(sampleRate);
	clock.setTick(sequencer->getPlayStartTick());

	running = true;
}

void FrameSeq::startMetronome(int sampleRate) {
	if (running)
		return;

	metronome = true;
	start(sampleRate);
}

void FrameSeq::work(int nFrames)
{
    for (auto& e : eventsAfterNFrames)
    {
        if (!e.occupied.load()) continue;

        e.frameCounter += nFrames;

        if (e.frameCounter >= e.nFrames)
        {
            e.f();
            e.reset();
        }
    }

    tickFrameOffset = 0;

    if (!wasRunning && running)
    {
        sendMidiSyncMsg(sequencer->getPlayStartTick() == 0 ? ShortMessage::START : ShortMessage::CONTINUE);
        wasRunning = true;
    }
    else if (wasRunning && !running)
    {
        sendMidiSyncMsg(ShortMessage::STOP);
        wasRunning = false;
    }

    if (!running)
		return;

    updateTimeDisplay(nFrames);
    processTempoChange();

    auto controls = mpc.getControls();
	auto seq = sequencer->getCurrentlyPlayingSequence();

    for (int frameIndex = 0; frameIndex < nFrames; frameIndex++)
	{
		if (clock.proc())
		{
			tickFrameOffset = frameIndex;

            triggerClickIfNeeded();
            displayPunchRects();

			if (!metronome)
			{
				if (sequencer->isCountingIn())
				{
                    stopCountingInIfRequired();
                    continue;
				}

				if (getTickPosition() >= seq->getLastTick() - 1 &&
                    !sequencer->isSongModeEnabled() &&
                    sequencer->getNextSq() != -1)
				{
					seq = switchToNextSequence();
					continue;
				}
				else if (sequencer->isSongModeEnabled())
				{
				    if (processSongMode())
                    {
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
                    processSeqLoopDisabled();
				}
			}

            sequencer->playToTick(getTickPosition());

            processNoteRepeat();
        }
	}
}

int FrameSeq::getEventFrameOffset()
{
	return tickFrameOffset;
}

void FrameSeq::stop()
{
	if (!running)
		return;

	running = false;
	metronome = false;
	tickFrameOffset = 0;
}

bool FrameSeq::isRunning()
{
	return running;
}

int FrameSeq::getTickPosition()
{
	return clock.getTickPosition();
}

void FrameSeq::move(int newTickPos)
{
	// Ugly hack to make sure the expected clock position is shown when
    // sequencer->move(...) notifies observers.
    clock.setTick(newTickPos + 1);

    sequencer->move(newTickPos);
    clock.setTick(newTickPos);
}

void FrameSeq::repeatPad(int duration)
{
    auto track = sequencer->getActiveTrack();

    if (mpc.getLayeredScreen()->getCurrentScreenName() != "sequencer" ||
        track->getBus() == 0)
    {
        return;
    }

	auto controls = mpc.getControls();

	if (!controls)
		return;

  auto program = mpc.getSampler()->getProgram(mpc.getDrum(track->getBus() - 1)->getProgram());
  auto hardware = mpc.getHardware();
  auto fullLevel = hardware->getTopPanel()->isFullLevelEnabled();

    for (auto& p : mpc.getHardware()->getPads())
  {
    if (!p->isPressed()) continue;

    auto padIndex = p->getPadIndexWithBankWhenLastPressed();

    auto note = program->getNoteFromPad(padIndex);

    if (note != 34)
    {
        auto tick = getTickPosition();
        auto noteEvent = std::make_shared<NoteEvent>(note);
        noteEvent->setTick(tick);
        noteEvent->setNote(note);
        mpc::Util::setSliderNoteVariationParameters(mpc, noteEvent, program);
        noteEvent->setVelocity(fullLevel ? 127 : p->getPressure());
        noteEvent->setDuration(duration);

        noteEvent->getNoteOff()->setTick(tick + duration);
        noteEvent->getNoteOff()->setVelocity(0);
        auto eventFrame = getEventFrameOffset();

        MidiAdapter midiAdapter;

        midiAdapter.process(noteEvent->getNoteOff(), track->getBus() - 1, 0);
        auto voiceOverlap = program->getNoteParameters(note)->getVoiceOverlap();
        mpc.getMms()->mpcTransport(midiAdapter.get().lock().get(), 0, 0, 0, eventFrame, -1, -1);

        auto newVelo = static_cast<int>(noteEvent->getVelocity() * (track->getVelocityRatio() * 0.01));
        midiAdapter.process(noteEvent, track->getBus() - 1, newVelo);
        auto durationFrames = duration == -1 ? -1 : SeqUtil::ticksToFrames(duration, clock.getBpm(), clock.getSampleRate());

        mpc.getMms()->mpcTransport(midiAdapter.get().lock().get(), 0, noteEvent->getVariationType(), noteEvent->getVariationValue(), eventFrame, -1, voiceOverlap == 2 ? durationFrames : -1);
        p->notifyObservers(newVelo);

        if (sequencer->isRecordingOrOverdubbing())
        {
            track->insertEventWhileRetainingSort(noteEvent);
        }
    }
  }
}

std::shared_ptr<Sequence> FrameSeq::switchToNextSequence()
{
    sequencer->playToTick(getTickPosition());
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
        return;

    if (sequencer->isRecordingOrOverdubbing() && !countMetronomeScreen->getInRec() && !sequencer->isCountingIn())
        return;

    auto isStepEditor = mpc.getLayeredScreen()->getCurrentScreenName() == "step-editor";

    if (!isStepEditor &&
        sequencer->isPlaying() &&
        !sequencer->isRecordingOrOverdubbing() &&
        !countMetronomeScreen->getInPlay() &&
        !sequencer->isCountingIn())
        return;

    auto pos = getTickPosition();
    auto bar = sequencer->getCurrentBarIndex();
    auto seq = sequencer->getCurrentlyPlayingSequence();
    auto firstTickOfBar = seq->getFirstTickOfBar(bar);
    auto relativePos = pos - firstTickOfBar;

    if (isStepEditor && relativePos == 0)
    {
        return;
    }

    auto den = seq->getDenominator(bar);
    auto denTicks = (int)(96 * (4.0 / den));

    switch (countMetronomeScreen->getRate())
    {
        case 1:
            denTicks *= 2.0f / 3;
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

    if (relativePos % denTicks == 0)
    {
        NoteEvent clickEvent(1);
        clickEvent.setVelocity(relativePos == 0 ? 127 : 64);
        mpc.getSampler()->playMetronome(&clickEvent, getEventFrameOffset());
        return;
    }
}

void FrameSeq::sendMidiSyncMsg(unsigned char status)
{
    auto midiSyncMsg = status == ShortMessage::TIMING_CLOCK ? midiSyncClockMsg : midiSyncStartStopContinueMsg;

    midiSyncMsg->bufferPos = tickFrameOffset;

    if (syncScreen->getModeOut() > 0)
    {
        midiSyncMsg->setMessage(status);

        if (syncScreen->getOut() == 0 || syncScreen->getOut() == 2)
        {
            mpc.getMidiOutput()->enqueMessageOutputA(midiSyncMsg);
        }

        if (syncScreen->getOut() == 1 || syncScreen->getOut() == 2)
        {
            mpc.getMidiOutput()->enqueMessageOutputB(midiSyncMsg);
        }
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

bool FrameSeq::processSeqLoopEnabled()
{
    auto seq = sequencer->getCurrentlyPlayingSequence();

    if (getTickPosition() >= seq->getLoopEnd() - 1)
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

        sequencer->playToTick(getTickPosition());
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

void FrameSeq::processSeqLoopDisabled()
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

void FrameSeq::processNoteRepeat()
{
    auto controls = mpc.getControls();

    if (controls && (controls->isTapPressed() || controls->isNoteRepeatLocked()))
    {
        int tcValue = sequencer->getTickValues()[timingCorrectScreen->getNoteValue()];
        int swingPercentage = timingCorrectScreen->getSwing();
        int swingOffset = (int)((swingPercentage - 50) * (4.0 * 0.01) * (tcValue * 0.5));
        auto shiftTiming = timingCorrectScreen->getAmount() * (timingCorrectScreen->isShiftTimingLater() ? 1 : -1);
        auto tickPosWithShift = getTickPosition() - shiftTiming;

        if (tcValue == 24 || tcValue == 48)
        {
            if (tickPosWithShift % (tcValue * 2) == swingOffset + tcValue ||
                tickPosWithShift % (tcValue * 2) == 0)
            {
                repeatPad(tcValue);
            }
        }
        else if (tcValue != 1 && tickPosWithShift % tcValue == 0)
        {
            repeatPad(tcValue);
        }
    }
}

void FrameSeq::updateTimeDisplay(unsigned int nFrames)
{
    frameCounter += nFrames;

    if (frameCounter > 2048)
    {
        if (!sequencer->isCountingIn() && !metronome)
        {
            sequencer->notifyTimeDisplayRealtime();
            sequencer->notifyObservers(std::string("timesignature"));
        }

        frameCounter = 0;
    }
}

void FrameSeq::processTempoChange()
{
    double tempo = sequencer->getTempo();

    if (tempo != clock.getBpm())
    {
        clock.set_bpm(tempo);
        sequencer->notify("tempo");
    }
}

void FrameSeq::stopSequencer()
{
    auto seq = sequencer->getCurrentlyPlayingSequence();
    sequencer->playToTick(seq->getLastTick() - 1);
    sequencer->stop();
    move(0);
}

void FrameSeq::enqueEventAfterNFrames(std::function<void()> event, unsigned long nFrames)
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
