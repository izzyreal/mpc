#include "FrameSeq.hpp"

#include <Mpc.hpp>

#include "audiomidi/EventHandler.hpp"

#include <hardware/Hardware.hpp>
#include <hardware/HwPad.hpp>
#include <hardware/TopPanel.hpp>
#include <controls/GlobalReleaseControls.hpp>

#include <sequencer/Song.hpp>
#include <sequencer/Step.hpp>
#include <sequencer/Track.hpp>

#include <lcdgui/screens/window/TimingCorrectScreen.hpp>
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
using namespace std;

FrameSeq::FrameSeq(mpc::Mpc& mpc)
	: mpc(mpc)
{
	sequencer = mpc.getSequencer();
}

void FrameSeq::start(float sampleRate) {
	//MLOG("frameSeq starting with sampleRate " + to_string(sampleRate));

	if (running)
		return;

	clock.init(sampleRate);
	clock.setTick(sequencer.lock()->getPlayStartTick());
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

	if (!running)
		return;

	auto controls = mpc.getControls().lock();
	auto songScreen = mpc.screens->get<SongScreen>("song");
	auto lSequencer = sequencer.lock();


	frameCounter += nFrames;

	if (frameCounter > 2048)
	{
		if (!lSequencer->isCountingIn() && !metronome)
			lSequencer->notifyTimeDisplayRealtime();

		frameCounter = 0;
	}

	auto seq = lSequencer->getCurrentlyPlayingSequence().lock();
	double tempo = lSequencer->getTempo();

	if (tempo != clock.getBpm())
	{
		clock.set_bpm(tempo);
		lSequencer->notify("tempo");
	}

	auto punchScreen = mpc.screens->get<PunchScreen>("punch");

	auto punch = punchScreen->on && lSequencer->isRecordingOrOverdubbing();

	auto punchInTime = punchScreen->time0;
	auto punchOutTime = punchScreen->time1;
	bool punchIn = punchScreen->autoPunch == 0 || punchScreen->autoPunch == 2;
	bool punchOut = punchScreen->autoPunch == 1 || punchScreen->autoPunch == 2;

	auto sequencerScreen = mpc.screens->get<SequencerScreen>("sequencer");
	auto userScreen = mpc.screens->get<UserScreen>("user");

	for (int i = 0; i < nFrames; i++)
	{
		if (clock.proc())
		{
			tickFrameOffset = i;

			if (punch)
			{
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

			if (!metronome)
			{
				if (lSequencer->isCountingIn())
				{
                    const int start = seq->getBarLengthsInTicks()[seq->getFirstLoopBarIndex()];

                    if (getTickPosition() >= start - 1)
					{
						lSequencer->playToTick(start - 1);
						move(seq->isLoopEnabled() ? seq->getLoopStart() : 0);
						lSequencer->setCountingIn(false);
						continue;
					}
					else {
						lSequencer->playToTick(getTickPosition());
						continue;
					}
				}

				if (getTickPosition() >= seq->getLastTick() - 1 && !lSequencer->isSongModeEnabled() && lSequencer->getNextSq() != -1)
				{
					lSequencer->playToTick(getTickPosition());
					checkNextSq();
					sequencer.lock()->move(0);
					seq = lSequencer->getCurrentlyPlayingSequence().lock();
					seq->initLoop();
					move(0);
					continue;
				}
				else if (lSequencer->isSongModeEnabled())
				{
					if (getTickPosition() >= seq->getLastTick() - 1)
					{
						lSequencer->playToTick(seq->getLastTick() - 1);
						lSequencer->incrementPlayedStepRepetitions();
						auto song = lSequencer->getSong(songScreen->getActiveSongIndex()).lock();
						auto step = songScreen->getOffset() + 1;

						auto doneRepeating = lSequencer->getPlayedStepRepetitions() >= song->getStep(step).lock()->getRepeats();
						auto reachedLastStep = step == song->getStepCount() - 1;

						if (doneRepeating && songScreen->isLoopEnabled() && step == song->getLastStep())
						{
							lSequencer->playToTick(seq->getLastTick() - 1);
							lSequencer->resetPlayedStepRepetitions();
							songScreen->setOffset(song->getFirstStep() - 1);
							auto newStep = song->getStep(songScreen->getOffset() + 1).lock();

							if (!lSequencer->getSequence(newStep->getSequence()).lock()->isUsed())
                            {

                                lSequencer->playToTick(seq->getLastTick() - 1);
                                lSequencer->stop();
                                move(0);
                                continue;
                            }

							move(0);
						}
						else if (doneRepeating && reachedLastStep)
						{
							lSequencer->playToTick(seq->getLastTick() - 1);
							lSequencer->setEndOfSong(true);
							lSequencer->stop();
							lSequencer->move(0);
							continue;
						}
						else
						{
							lSequencer->playToTick(seq->getLastTick() - 1);

							if (doneRepeating)
							{
								lSequencer->resetPlayedStepRepetitions();
								songScreen->setOffset(songScreen->getOffset() + 1);

								auto newStep = song->getStep(songScreen->getOffset() + 1).lock();

								if (!lSequencer->getSequence(newStep->getSequence()).lock()->isUsed())
                                {
                                    lSequencer->playToTick(seq->getLastTick() - 1);
                                    lSequencer->stop();
                                    move(0);
                                    continue;
                                }
							}

							move(0);
						}
					}
				}
				else if (seq->isLoopEnabled())
				{
					if (getTickPosition() >= seq->getLoopEnd() - 1)
					{
						if (punch && punchIn)
						{
							sequencerScreen->setPunchRectOn(0, true);
							sequencerScreen->setPunchRectOn(1, false);
						}

						if (punch && punchOut)
							sequencerScreen->setPunchRectOn(2, false);

						if (punch && punchOut && !punchIn)
							sequencerScreen->setPunchRectOn(1, true);

						lSequencer->playToTick(getTickPosition());
						move(seq->getLoopStart());

						if (lSequencer->isRecordingOrOverdubbing())
						{
							if (lSequencer->isRecording())
								lSequencer->switchRecordToOverDub();

							if (lSequencer->isRecordingModeMulti())
							{
								for (auto& t : seq->getTracks())
									t.lock()->removeDoubles();
							}
							else
							{
								seq->getTrack(lSequencer->getActiveTrackIndex()).lock()->removeDoubles();
							}
						}

						continue;
					}
				}
				else
				{
					if (getTickPosition() >= seq->getLastTick())
					{
						if (lSequencer->isRecordingOrOverdubbing())
						{
							seq->insertBars(1, seq->getLastBarIndex());
							seq->setTimeSignature(seq->getLastBarIndex(), seq->getLastBarIndex(), userScreen->timeSig.getNumerator(), userScreen->timeSig.getDenominator());
						}
						else
						{
							lSequencer->stop(seq->getLastTick());
							lSequencer->move(seq->getLastTick());
						}
					}
				}
			}

            lSequencer->playToTick(getTickPosition());

            if (controls && (controls->isTapPressed() || controls->isNoteRepeatLocked()))
            {
                auto timingCorrectScreen = mpc.screens->get<TimingCorrectScreen>("timing-correct");
                int tcValue = lSequencer->getTickValues()[timingCorrectScreen->getNoteValue()];
                int swingPercentage = timingCorrectScreen->getSwing();
                int swingOffset = (int)((swingPercentage - 50) * (4.0 * 0.01) * (tcValue * 0.5));

                if (tcValue == 24 || tcValue == 48)
                {
                    if (getTickPosition() % (tcValue * 2) == swingOffset + tcValue ||
                        getTickPosition() % (tcValue * 2) == 0)
                    {
                        repeatPad(getTickPosition(), tcValue);
                    }
                }
                else
                {
                    if (tcValue != 1 && getTickPosition() % tcValue == 0)
                    {
                        repeatPad(getTickPosition(), tcValue);
                    }
                }
            }
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
	sequencer.lock()->move(newTickPos);
	clock.setTick(newTickPos);
}

void FrameSeq::repeatPad(int tick, int duration)
{
    auto track = sequencer.lock()->getActiveTrack().lock();

    if (mpc.getLayeredScreen().lock()->getCurrentScreenName() != "sequencer" ||
        track->getBus() == 0)
    {
        return;
    }

	auto controls = mpc.getControls().lock();

	if (!controls)
		return;

  auto program = mpc.getSampler().lock()->getProgram(mpc.getDrum(track->getBus() - 1)->getProgram()).lock();
  auto hardware = mpc.getHardware().lock();
  auto fullLevel = hardware->getTopPanel().lock()->isFullLevelEnabled();

    for (auto& p : mpc.getHardware().lock()->getPads())
  {
    if (!p->isPressed()) continue;

    auto padIndex = p->getPadIndexWithBankWhenLastPressed();

    auto note = program->getNoteFromPad(padIndex);

    if (note != 34)
    {
        auto noteEvent = std::make_shared<NoteEvent>(note);
        noteEvent->setTick(tick);
        noteEvent->setNote(note);
        mpc::Util::setSliderNoteVariationParameters(mpc, noteEvent, program);
        noteEvent->setVelocity(fullLevel ? 127 : p->getPressure());
        noteEvent->setDuration(duration);

        noteEvent->getNoteOff().lock()->setTick(tick + duration);
        noteEvent->getNoteOff().lock()->setVelocity(0);
        auto eventFrame = getEventFrameOffset();

        MidiAdapter midiAdapter;

        midiAdapter.process(noteEvent->getNoteOff(), track->getBus() - 1, 0);
        mpc.getMms()->mpcTransport(midiAdapter.get().lock().get(), 0, 0, 0, eventFrame, -1);

        auto newVelo = static_cast<int>(noteEvent->getVelocity() * (track->getVelocityRatio() * 0.01));
        midiAdapter.process(noteEvent, track->getBus() - 1, newVelo);
        mpc.getMms()->mpcTransport(midiAdapter.get().lock().get(), 0, noteEvent->getVariationType(), noteEvent->getVariationValue(), eventFrame, -1);
        p->notifyObservers(newVelo);

        if (sequencer.lock()->isRecordingOrOverdubbing())
        {
            track->insertEventWhileRetainingSort(noteEvent);
        }
    }
  }
}

void FrameSeq::checkNextSq()
{
	auto lSeq = sequencer.lock();
	lSeq->setCurrentlyPlayingSequenceIndex(lSeq->getNextSq());
	lSeq->setNextSq(-1);
	lSeq->notify("nextsqoff");
	lSeq->notify("seqnumbername");
    lSeq->notify("timesignature");
    lSeq->notify("numberofbars");
    lSeq->notify("tempo");
    lSeq->notify("loop");
}
