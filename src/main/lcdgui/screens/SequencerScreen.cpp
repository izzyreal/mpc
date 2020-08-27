#include "SequencerScreen.hpp"

#include <sequencer/Track.hpp>
#include <sequencer/TempoChangeEvent.hpp>
#include <sequencer/NoteEvent.hpp>
#include <sequencer/TimeSignature.hpp>

#include <lcdgui/screens/window/TimingCorrectScreen.hpp>
#include <lcdgui/screens/StepEditorScreen.hpp>
#include <lcdgui/screens/UserScreen.hpp>
#include <lcdgui/screens/PunchScreen.hpp>
#include <lcdgui/PunchRect.hpp>

#include <sequencer/SeqUtil.hpp>
#include <Util.hpp>

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;
using namespace mpc::sequencer;
using namespace moduru::lang;
using namespace std;

SequencerScreen::SequencerScreen(mpc::Mpc& mpc, const int layerIndex)
	: ScreenComponent(mpc, "sequencer", layerIndex)
{
	MRECT punch0(0, 52, 30, 59);
	addChildT<PunchRect>("punch-rect-0", punch0).lock()->Hide(true);

	MRECT punch1(105, 52, 135, 59);
	addChildT<PunchRect>("punch-rect-1", punch1).lock()->Hide(true);

	MRECT punch2(217, 52, 247, 59);
	addChildT<PunchRect>("punch-rect-2", punch2).lock()->Hide(true);
}

void SequencerScreen::open()
{
	findField("loop").lock()->setAlignment(Alignment::Centered);
	findField("on").lock()->setAlignment(Alignment::Centered);
	findField("bars").lock()->setAlignment(Alignment::Centered);
	findField("pgm").lock()->setAlignment(Alignment::Centered);
	findField("count").lock()->setAlignment(Alignment::Centered);

	findLabel("tempo").lock()->setSize(18, 9);
	init();
	sequence = sequencer.lock()->getActiveSequence();
	track = sequencer.lock()->getActiveTrack();

	findLabel("punch-time-0").lock()->Hide(true);
	findLabel("punch-time-1").lock()->Hide(true);

	displaySq();
	displayTr();
	displayOn();
	displayCount();
	displayTiming();
	displayLoop();
	displayRecordingMode();
	displayBars();
	displayNow0();
	displayNow1();
	displayNow2();
	displayTempo();
	displayTempoSource();
	displayTsig();
	displayPgm();
	displayVelo();
	displayBus();
	displayDeviceNumber();
	displayNextSq();

	sequencer.lock()->addObserver(this);
	sequence.lock()->addObserver(this);
	track.lock()->addObserver(this);

	findChild<TextComp>("fk3").lock()->setBlinking(sequencer.lock()->isSoloEnabled());

	auto punchScreen = dynamic_pointer_cast<PunchScreen>(mpc.screens->getScreenComponent("punch"));

	findChild("function-keys").lock()->Hide(punchScreen->on);

	if (sequencer.lock()->isSecondSequenceEnabled())
		findBackground().lock()->setName("sequencer-2nd");
	else if (punchScreen->on && !sequencer.lock()->isRecordingOrOverdubbing())
		findBackground().lock()->setName("sequencer-punch-active");
	else
		findBackground().lock()->setName("sequencer");

	if (sequencer.lock()->getNextSq() != -1)
		ls.lock()->setFocus("nextsq");
}

void SequencerScreen::close()
{
	vector<string> screensThatDisablePunch{ "song", "load", "save", "others", "next-seq" };
	auto nextScreen = ls.lock()->getCurrentScreenName();

	if (find(begin(screensThatDisablePunch), end(screensThatDisablePunch), nextScreen) != end(screensThatDisablePunch))
	{
		auto punchScreen = dynamic_pointer_cast<PunchScreen>(mpc.screens->getScreenComponent("punch"));
		punchScreen->on = false;
	}

	sequencer.lock()->deleteObserver(this);
	sequence.lock()->deleteObserver(this);
	track.lock()->deleteObserver(this);
}

void SequencerScreen::displayVelo()
{
	findField("velo").lock()->setTextPadded(to_string(sequencer.lock()->getActiveTrack().lock()->getVelocityRatio()));
}

void SequencerScreen::displayDeviceNumber()
{
	if (track.lock()->getDevice() == 0)
	{
		findField("devicenumber").lock()->setText("OFF");
	}
	else
	{
		if (track.lock()->getDevice() >= 17)
			findField("devicenumber").lock()->setText(to_string(track.lock()->getDevice() - 16) + "B");
		else
			findField("devicenumber").lock()->setText(to_string(track.lock()->getDevice()) + "A");
	}
}

vector<string> SequencerScreen::busNames = vector<string>{ "MIDI", "DRUM1", "DRUM2", "DRUM3", "DRUM4" };

void SequencerScreen::displayBus()
{
	findField("bus").lock()->setText(busNames[sequencer.lock()->getActiveTrack().lock()->getBus()]);
	displayDeviceName();
}

void SequencerScreen::displayBars()
{
	findField("bars").lock()->setText(to_string(sequencer.lock()->getActiveSequence().lock()->getLastBarIndex() + 1));
}

void SequencerScreen::displayPgm()
{
	if (track.lock()->getProgramChange() == 0)
		findField("pgm").lock()->setText("OFF");
	else
		findField("pgm").lock()->setText(to_string(track.lock()->getProgramChange()));
}

void SequencerScreen::displayDeviceName()
{
	if (track.lock()->getBus() != 0)
	{
		if (track.lock()->getDevice() == 0)
		{
			int pgm = sampler.lock()->getDrumBusProgramNumber(track.lock()->getBus());
			auto p = dynamic_pointer_cast<mpc::sampler::Program>(sampler.lock()->getProgram(pgm).lock());
			findLabel("devicename").lock()->setText(p->getName());
		}
		else
		{
			findLabel("devicename").lock()->setText(sequencer.lock()->getActiveSequence().lock()->getDeviceName(track.lock()->getDevice()));
		}
	}
	else if (track.lock()->getBus() == 0)
	{
		if (track.lock()->getDevice() == 0)
			findLabel("devicename").lock()->setText("NewPgm-A");
		else
			findLabel("devicename").lock()->setText(sequencer.lock()->getActiveSequence().lock()->getDeviceName(track.lock()->getDevice()));
	}
}

void SequencerScreen::displayTempo()
{
	displayTempoLabel();
	findField("tempo").lock()->setText(StrUtil::padLeft(Util::tempoString(sequencer.lock()->getTempo()), " ", 6));
}

void SequencerScreen::displayTempoLabel()
{
	auto currentRatio = -1;
	auto sequence = sequencer.lock()->getActiveSequence().lock();

	if (!sequence->isUsed() || !sequence->isTempoChangeOn())
	{
		findLabel("tempo").lock()->setText(u8" \u00C0:");
		return;
	}

	for (auto& tce : sequence->getTempoChangeEvents())
	{
		auto lTce = tce.lock();
		
		if (lTce->getTick() > sequencer.lock()->getTickPosition())
			break;
	
		currentRatio = lTce->getRatio();
	}

	if (currentRatio != 1000)
		findLabel("tempo").lock()->setText(u8"c\u00C0:");
	else
		findLabel("tempo").lock()->setText(u8" \u00C0:");
}

void SequencerScreen::displayTempoSource()
{
	findField("tempo-source").lock()->setText(sequencer.lock()->isTempoSourceSequenceEnabled() ? "(SEQ)" : "(MAS)");
}


void SequencerScreen::displaySq()
{
	string result = "";

	if (sequencer.lock()->isPlaying())
	{
		result.append(StrUtil::padLeft(to_string(sequencer.lock()->getCurrentlyPlayingSequenceIndex() + 1), "0", 2));
		result.append("-");
		result.append(sequencer.lock()->getCurrentlyPlayingSequence().lock()->getName());
		findField("sq").lock()->setText(result);
	}
	else
	{
		result.append(StrUtil::padLeft(to_string(sequencer.lock()->getActiveSequenceIndex() + 1), "0", 2));
		result.append("-");
		result.append(sequencer.lock()->getActiveSequence().lock()->getName());
		findField("sq").lock()->setText(result);
	}
}

void SequencerScreen::displayNow0()
{
	findField("now0").lock()->setTextPadded(sequencer.lock()->getCurrentBarIndex() + 1, "0");
}

void SequencerScreen::displayNow1()
{
	findField("now1").lock()->setTextPadded(sequencer.lock()->getCurrentBeatIndex() + 1, "0");
}

void SequencerScreen::displayNow2()
{
	findField("now2").lock()->setTextPadded(sequencer.lock()->getCurrentClockNumber(), "0");
}

void SequencerScreen::displayRecordingMode()
{
	findField("recordingmode").lock()->setText(sequencer.lock()->isRecordingModeMulti() ? "M" : "S");
}

void SequencerScreen::displayTsig()
{
	string result = "";
	auto ts = sequence.lock()->getTimeSignature();
	result.append(to_string(ts.getNumerator()));
	result.append("/");
	result.append(to_string(ts.getDenominator()));
	findField("tsig").lock()->setText(mpc::Util::distributeTimeSig(result));
}

void SequencerScreen::displayLoop()
{
	findField("loop").lock()->setText(sequence.lock()->isLoopEnabled() ? "ON" : "OFF");
}

void SequencerScreen::displayOn() {
	findField("on").lock()->setText(sequencer.lock()->getActiveTrack().lock()->isOn() ? "YES" : "NO");
}

void SequencerScreen::displayTr() {
	
	string result = StrUtil::padLeft(to_string(sequencer.lock()->getActiveTrackIndex() + 1), "0", 2);
	result.append("-");
	result.append(sequencer.lock()->getActiveTrack().lock()->getName());
	findField("tr").lock()->setText(result);
}

void SequencerScreen::displayCount()
{
	findField("count").lock()->setText(sequencer.lock()->isCountEnabled() ? "ON" : "OFF");
}

vector<string> SequencerScreen::timingCorrectNames = vector<string>{ "OFF", "1/8", "1/8(3)", "1/16", "1/16(3)", "1/32", "1/32(3)" };

void SequencerScreen::displayTiming()
{
	auto noteValue = dynamic_pointer_cast<TimingCorrectScreen>(mpc.screens->getScreenComponent("timing-correct"))->getNoteValue();
	findField("timing").lock()->setText(timingCorrectNames[noteValue]);
}

void SequencerScreen::update(moduru::observer::Observable* o, nonstd::any arg)
{
	track.lock()->deleteObserver(this);
	sequence.lock()->deleteObserver(this);
	
	sequence = sequencer.lock()->getActiveSequence();
	track = sequencer.lock()->getActiveTrack();

	sequence.lock()->addObserver(this);
	track.lock()->addObserver(this);

	string s = nonstd::any_cast<string>(arg);

	if (s.compare("nextsqvalue") == 0)
	{
		displayNextSq();
	}
	else if (s.compare("nextsq") == 0)
	{
		displayNextSq();
		ls.lock()->setFocus("nextsq");
	}
	else if (s.compare("nextsqoff") == 0)
	{
		ls.lock()->setFocus("sq");
		displayNextSq();
	}
	else if (s.compare("count") == 0)
	{
		displayCount();
	}
	else if (s.compare("tracknumbername") == 0)
	{
		displayTr();
		displayOn();
	}
	else if (s.compare("seqnumbername") == 0)
	{
		displaySq();
	}
	else if (s.compare("loop") == 0)
	{
		displayLoop();
	}
	else if (s.compare("recordingmode") == 0)
	{
		displayRecordingMode();
	}
	else if (s.compare("numberofbars") == 0)
	{
		displayBars();
	}
	else if (s.compare("trackon") == 0)
	{
		displayOn();
	}
	else if (s.compare("bar") == 0)
	{
		displayNow0();
		displayTempoLabel();
	}
	else if (s.compare("beat") == 0)
	{
		displayNow1();
		displayTempoLabel();
	}
	else if (s.compare("clock") == 0)
	{
		displayNow2();
		displayTempoLabel();
	}
	else if (s.compare("tempo") == 0)
	{
		displayTempo();
	}
	else if (s.compare("tempo-source") == 0)
	{
		displayTempoSource();
	}
	else if (s.compare("timesignature") == 0)
	{
		displayTsig();
	}
	else if (s.compare("programchange") == 0)
	{
		displayPgm();
	}
	else if (s.compare("velocityratio") == 0)
	{
		displayVelo();
	}
	else if (s.compare("bus") == 0)
	{
		displayBus();
	}
	else if (s.compare("device") == 0)
	{
		displayDeviceNumber();
	}
	else if (s.compare("devicename") == 0)
	{
		displayDeviceName();
	}
}

void SequencerScreen::pressEnter()
{
	init();
	baseControls->pressEnter();

	auto focusedField = findChild<Field>(param).lock();

	if (!focusedField->isTypeModeEnabled())
		return;

	auto candidate = focusedField->enter();

	if (candidate != INT_MAX)
	{
		if (param.compare("now0") == 0)
		{
			sequencer.lock()->setBar(candidate - 1);
			setLastFocus("step-editor", "view");
		}
		else if (param.compare("now1") == 0)
		{
			sequencer.lock()->setBeat(candidate - 1);
			setLastFocus("step-editor", "view");
		}
		else if (param.compare("now2") == 0)
		{
			sequencer.lock()->setClock(candidate);
			setLastFocus("step-editor", "view");
		}
		else if (param.compare("tempo") == 0)
		{
			sequencer.lock()->setTempo(candidate * 0.1);
		}
		else if (param.compare("velo") == 0)
		{
			track.lock()->setVelocityRatio(candidate);
		}
	}
}

void SequencerScreen::function(int i)
{
	init();
	ScreenComponent::function(i);
	auto punchScreen = dynamic_pointer_cast<PunchScreen>(mpc.screens->getScreenComponent("punch"));

	if (punchScreen->on)
	{
		if (!sequencer.lock()->isRecordingOrOverdubbing() && i == 5)
		{
			punchScreen->on = false;
			findBackground().lock()->setName("sequencer");
			findChild("function-keys").lock()->Hide(false);
		}
		return;
	}

	switch (i)
	{
	case 0:
		if (sequencer.lock()->isPlaying())
			return;

		Util::initSequence(mpc);

		openScreen("step-editor");
		break;
	case 1:
	{
		if (sequencer.lock()->isPlaying())
			return;

		openScreen("events");
		break;
	}
	case 2:
		track.lock()->setOn(!track.lock()->isOn());
		break;
	case 3:
	{
		sequencer.lock()->setSoloEnabled(!sequencer.lock()->isSoloEnabled());
		findChild<TextComp>("fk3").lock()->setBlinking(sequencer.lock()->isSoloEnabled());
		break;
	}
	case 4:
		sequencer.lock()->trackDown();
		break;
	case 5:
		sequencer.lock()->trackUp();
		break;
	}
}

void SequencerScreen::checkTrackUsed()
{
	if (!track.lock()->isUsed())
	{
		track.lock()->setUsed(true);
		displayTr();
	}
}

void SequencerScreen::turnWheel(int i)
{
	init();

	if (param.size() >= 3 && param.substr(0, 3).compare("now") == 0)
		setLastFocus("step-editor", "view");

	if (param.compare("now0") == 0)
	{
		sequencer.lock()->setBar(sequencer.lock()->getCurrentBarIndex() + i);
	}
	else if (param.compare("now1") == 0)
	{
		sequencer.lock()->setBeat(sequencer.lock()->getCurrentBeatIndex() + i);
	}
	else if (param.compare("now2") == 0)
	{
		sequencer.lock()->setClock(sequencer.lock()->getCurrentClockNumber() + i);
	}
	else if (param.compare("devicenumber") == 0)
	{
		checkTrackUsed();
		track.lock()->setDeviceNumber(track.lock()->getDevice() + i);
	}
	else if (param.compare("tr") == 0)
	{
		if (i > 0)
		{
			sequencer.lock()->trackUp();
		}
		else if (i < 0)
		{
			sequencer.lock()->trackDown();
		}
	}
	else if (param.compare("bus") == 0)
	{
		checkTrackUsed();

		track.lock()->setBusNumber(track.lock()->getBus() + i);
		
		auto lastFocus = getLastFocus("step-editor");

		if (lastFocus.length() == 2)
		{
			auto eventNumber = stoi(lastFocus.substr(1, 2));

			auto stepEditorScreen = dynamic_pointer_cast<StepEditorScreen>(mpc.screens->getScreenComponent("step-editor"));

			if (dynamic_pointer_cast<mpc::sequencer::NoteEvent>(stepEditorScreen->getVisibleEvents()[eventNumber].lock()))
			{
				if (track.lock()->getBus() == 0)
				{
					if (lastFocus[0] == 'd' || lastFocus[0] == 'e')
					{
						setLastFocus("step-editor", "a" + to_string(eventNumber));
					}
				}
			}
		}
	}
	else if (param.compare("pgm") == 0)
	{
		checkTrackUsed();
		track.lock()->setProgramChange(track.lock()->getProgramChange() + i);
	}
	else if (param.compare("velo") == 0)
	{
		checkTrackUsed();
		track.lock()->setVelocityRatio(track.lock()->getVelocityRatio() + i);
	}
	else if (param.compare("timing") == 0)
	{
		auto screen = mpc.screens->get<TimingCorrectScreen>("timing-correct");
		auto noteValue = screen->getNoteValue();
		screen->setNoteValue(noteValue + i);
		setLastFocus("timing-correct", "notevalue");
		displayTiming();
	}
	else if (param.compare("sq") == 0)
	{
		auto punchScreen = dynamic_pointer_cast<PunchScreen>(mpc.screens->getScreenComponent("punch"));
		
		if (sequencer.lock()->isPlaying())
		{
			if (!punchScreen->on)
				sequencer.lock()->setNextSq(sequencer.lock()->getCurrentlyPlayingSequenceIndex() + i);
		}
		else
		{
			if (punchScreen->on)
			{
				punchScreen->on = false;
				findBackground().lock()->setName("sequencer");
				findChild("function-keys").lock()->Hide(false);
			}

			sequencer.lock()->setActiveSequenceIndex(sequencer.lock()->getActiveSequenceIndex() + i);
		}
	}
	else if (param.compare("nextsq") == 0)
	{
		sequencer.lock()->setNextSq(sequencer.lock()->getNextSq() + i);
	}
	else if (param.compare("bars") == 0)
	{
		openScreen("change-bars-2");
	}
	else if (param.compare("tempo") == 0)
	{
		double oldTempo = sequencer.lock()->getTempo();
		double newTempo = oldTempo + (i * 0.1);
		sequencer.lock()->setTempo(newTempo);
	}
	else if (param.compare("tsig") == 0)
	{
		openScreen("change-tsig");
	}
	else if (param.compare("tempo-source") == 0)
	{
		sequencer.lock()->setTempoSourceSequence(i > 0);
	}
	else if (param.compare("count") == 0)
	{
		sequencer.lock()->setCountEnabled(i > 0);
	}
	else if (param.compare("loop") == 0)
	{
		sequence.lock()->setLoopEnabled(i > 0);
	}
	else if (param.compare("recordingmode") == 0)
	{
		sequencer.lock()->setRecordingModeMulti(i > 0);
	}
	else if (param.compare("on") == 0)
	{
		checkTrackUsed();
		track.lock()->setOn(i > 0);
	}
}

void SequencerScreen::openWindow()
{
	init();

	if (sequencer.lock()->isPlaying())
		return;
	
	if (param.compare("sq") == 0)
	{
		Util::initSequence(mpc);
		openScreen("sequence");
	}
	else if (param.find("now") != string::npos)
	{
		openScreen("time-display");
	}
	else if (param.find("tempo") != string::npos)
	{
		openScreen("tempo-change");
	}
	else if (param.compare("timing") == 0)
	{
		openScreen("timing-correct");
	}
	else if (param.compare("tsig") == 0)
	{
		openScreen("change-tsig");
	}
	else if (param.compare("count") == 0)
	{
		openScreen("count-metronome");
	}
	else if (param.compare("loop") == 0)
	{
		openScreen("loop-bars-window");
	}
	else if (param.compare("tr") == 0)
	{
		if (!track.lock()->isUsed())
			track.lock()->setUsed(true);

		openScreen("track");
	}
	else if (param.compare("on") == 0)
	{
		openScreen("erase-all-off-tracks");
	}
	else if (param.compare("pgm") == 0)
	{
		openScreen("transmit-program-changes");
	}
	else if (param.compare("recordingmode") == 0)
	{
		openScreen("multi-recording-setup");
	}
	else if (param.compare("bus") == 0)
	{
		openScreen("midi-input");
	}
	else if (param.compare("devicenumber") == 0)
	{
		openScreen("midi-output");
	}
	else if (param.compare("bars") == 0)
	{
		openScreen("change-bars");
	}
	else if (param.compare("velo") == 0) {
		openScreen("edit-velocity");
	}
}

void SequencerScreen::left()
{
	init();

	if (sequencer.lock()->getNextSq() != -1)
		return;

	ScreenComponent::left();
}

void SequencerScreen::right()
{
	init();

	if (sequencer.lock()->getNextSq() != -1)
		return;

	ScreenComponent::right();
}

void SequencerScreen::up()
{
	init();

	if (sequencer.lock()->getNextSq() != -1)
		return;

	ScreenComponent::up();
}

void SequencerScreen::down()
{
	init();

	if (sequencer.lock()->getNextSq() != -1)
		return;

	ScreenComponent::down();
}

void SequencerScreen::setPunchRectOn(int i, bool b)
{
	findChild<PunchRect>("punch-rect-" + to_string(i)).lock()->setOn(b);
}

void SequencerScreen::displayPunchWhileRecording()
{
	auto punchScreen = dynamic_pointer_cast<PunchScreen>(mpc.screens->getScreenComponent("punch"));
	auto controls = mpc.getControls().lock();

	if (punchScreen->on && (controls->isRecPressed() || controls->isOverDubPressed()))
	{
		findBackground().lock()->setName("sequencer");

		for (int i = 0; i < 3; i++)
		{
			auto punchRect = findChild<PunchRect>("punch-rect-" + to_string(i)).lock();
			punchRect->Hide((i == 0 && punchScreen->autoPunch == 1) || (i == 2 && punchScreen->autoPunch == 0));
			punchRect->setOn((i == 0 && punchScreen->autoPunch != 1) || (i == 1 && punchScreen->autoPunch == 1));
		}

		auto time0 = findLabel("punch-time-0").lock();
		auto time1 = findLabel("punch-time-1").lock();

		time0->Hide(punchScreen->autoPunch == 1);
		time1->Hide(punchScreen->autoPunch == 0);

		init();

		auto seq = sequence.lock();

		auto text1 = StrUtil::padLeft(to_string(SeqUtil::getBar(seq.get(), punchScreen->time0) + 1), "0", 3);
		auto text2 = StrUtil::padLeft(to_string(SeqUtil::getBeat(seq.get(), punchScreen->time0) + 1), "0", 2);
		auto text3 = StrUtil::padLeft(to_string(SeqUtil::getClock(seq.get(), punchScreen->time0)), "0", 2);
		auto text4 = StrUtil::padLeft(to_string(SeqUtil::getBar(seq.get(), punchScreen->time1) + 1), "0", 3);
		auto text5 = StrUtil::padLeft(to_string(SeqUtil::getBeat(seq.get(), punchScreen->time1) + 1), "0", 2);
		auto text6 = StrUtil::padLeft(to_string(SeqUtil::getClock(seq.get(), punchScreen->time1)), "0", 2);

		time0->setText("IN:" + text1 + "." + text2 + "." + text3);
		time1->setText("OUT:" + text4 + "." + text5 + "." + text6);
	}
}

void SequencerScreen::displayNextSq()
{
	ls.lock()->setFunctionKeysArrangement(sequencer.lock()->getNextSq() == -1 ? 0 : 1);

	auto noNextSq = sequencer.lock()->getNextSq() == -1;
	findLabel("nextsq").lock()->Hide(noNextSq);
	findField("nextsq").lock()->Hide(noNextSq);

	if (noNextSq)
		return;

	findField("nextsq").lock()->setTextPadded(sequencer.lock()->getNextSq() + 1);
}

void SequencerScreen::play()
{
	if (sequencer.lock()->isPlaying())
		return;

	displayPunchWhileRecording();

	ScreenComponent::play();
}

void SequencerScreen::playStart()
{
	if (sequencer.lock()->isPlaying())
		return;

	displayPunchWhileRecording();

	ScreenComponent::playStart();
}

void SequencerScreen::stop()
{
	auto punchScreen = dynamic_pointer_cast<PunchScreen>(mpc.screens->getScreenComponent("punch"));

	if (punchScreen->on)
	{
		findBackground().lock()->setName("sequencer-punch-active");

		for (int i = 0; i < 3; i++)
			findChild<PunchRect>("punch-rect-" + to_string(i)).lock()->Hide(true);
		
		auto time0 = findLabel("punch-time-0").lock();
		auto time1 = findLabel("punch-time-1").lock();

		time0->Hide(true);
		time1->Hide(true);

	}

	ScreenComponent::stop();
}

void SequencerScreen::rec()
{
	Util::initSequence(mpc);
	ScreenComponent::rec();
}

void SequencerScreen::overDub()
{
	Util::initSequence(mpc);
	ScreenComponent::overDub();
}
