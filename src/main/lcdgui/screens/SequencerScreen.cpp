#include "SequencerScreen.hpp"

#include <sequencer/Track.hpp>
#include <sequencer/TempoChangeEvent.hpp>
#include <sequencer/NoteEvent.hpp>
#include <sequencer/TimeSignature.hpp>

#include <lcdgui/screens/window/TimingCorrectScreen.hpp>
#include <lcdgui/screens/EventsScreen.hpp>
#include <lcdgui/screens/StepEditorScreen.hpp>
#include <lcdgui/screens/UserScreen.hpp>

#include <Util.hpp>

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;

SequencerScreen::SequencerScreen(const int layerIndex)
	: ScreenComponent("sequencer", layerIndex)
{
}

void SequencerScreen::open()
{

	sequence = sequencer.lock()->getActiveSequence();
	track = sequencer.lock()->getActiveTrack();

	findLabel("nextsq").lock()->Hide(true);
	findField("nextsq").lock()->Hide(true);

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

	sequencer.lock()->addObserver(this);
	sequence.lock()->addObserver(this);
	track.lock()->addObserver(this);
}

void SequencerScreen::close()
{
	sequencer.lock()->deleteObserver(this);
	sequence.lock()->deleteObserver(this);
	track.lock()->deleteObserver(this);
}

void SequencerScreen::displayVelo()
{
	findField("velo").lock()->setText(to_string(sequencer.lock()->getActiveTrack().lock()->getVelocityRatio()));
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
		{
			findField("devicenumber").lock()->setText(to_string(track.lock()->getDevice() - 16) + "B");
		}
		else
		{
			findField("devicenumber").lock()->setText(to_string(track.lock()->getDevice()) + "A");
		}
	}
}

vector<string> SequencerScreen::busNames = vector<string>{ "MIDI", "DRUM1", "DRUM2", "DRUM3", "DRUM4" };

void SequencerScreen::displayBus()
{
	findField("tracktype").lock()->setText(busNames[sequencer.lock()->getActiveTrack().lock()->getBusNumber()]);
	displayDeviceName();
}

void SequencerScreen::displayBars()
{
	findField("bars").lock()->setText(to_string(sequencer.lock()->getActiveSequence().lock()->getLastBar() + 1));
}

void SequencerScreen::displayPgm()
{
	if (track.lock()->getProgramChange() == 0)
	{
		findField("pgm").lock()->setText("OFF");
	}
	else
	{
		findField("pgm").lock()->setText(to_string(track.lock()->getProgramChange()));
	}
}

void SequencerScreen::displayDeviceName()
{
	if (track.lock()->getBusNumber() != 0)
	{
		if (track.lock()->getDevice() == 0)
		{
			int pgm = sampler.lock()->getDrumBusProgramNumber(track.lock()->getBusNumber());
			auto p = dynamic_pointer_cast<mpc::sampler::Program>(sampler.lock()->getProgram(pgm).lock());
			findLabel("devicename").lock()->setText(p->getName());
		}
		else
		{
			findLabel("devicename").lock()->setText(sequencer.lock()->getActiveSequence().lock()->getDeviceName(track.lock()->getDevice()));
		}
	}
	else if (track.lock()->getBusNumber() == 0)
	{
		if (track.lock()->getDevice() == 0)
		{
			findLabel("devicename").lock()->setText("NewPgm-A");
		}
		else
		{
			findLabel("devicename").lock()->setText(sequencer.lock()->getActiveSequence().lock()->getDeviceName(track.lock()->getDevice()));
		}
	}
}

void SequencerScreen::displayTempo()
{
	displayTempoLabel();
	findField("tempo").lock()->setText(Util::tempoString(sequencer.lock()->getTempo()));
}

void SequencerScreen::displayTempoLabel()
{
	auto currentRatio = -1;
	for (auto& tce : sequencer.lock()->getActiveSequence().lock()->getTempoChangeEvents())
	{
		auto lTce = tce.lock();
		if (lTce->getTick() > sequencer.lock()->getTickPosition())
		{
			break;
		}
		currentRatio = lTce->getRatio();
	}
	if (currentRatio != 1000)
	{
		findLabel("tempo").lock()->setText(u8"c\u00C0:");
	}
	else
	{
		findLabel("tempo").lock()->setText(u8" \u00C0:");
	}
}

void SequencerScreen::displayTempoSource()
{
	findField("temposource").lock()->setText(sequencer.lock()->isTempoSourceSequenceEnabled() ? "(SEQ)" : "(MAS)");
}


void SequencerScreen::displaySq()
{
	string result = "";

	if (sequencer.lock()->isPlaying()) {
		result.append(moduru::lang::StrUtil::padLeft(to_string(sequencer.lock()->getCurrentlyPlayingSequenceIndex() + 1), "0", 2));
		result.append("-");
		result.append(sequencer.lock()->getCurrentlyPlayingSequence().lock()->getName());
		findField("sq").lock()->setText(result);
	}
	else {
		result.append(moduru::lang::StrUtil::padLeft(to_string(sequencer.lock()->getActiveSequenceIndex() + 1), "0", 2));
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
	
	string result = moduru::lang::StrUtil::padLeft(to_string(sequencer.lock()->getActiveTrackIndex() + 1), "0", 2);
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
	auto noteValue = dynamic_pointer_cast<TimingCorrectScreen>(Screens::getScreenComponent("timing-correct"))->getNoteValue();
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

	auto nextSqField = findField("nextsq").lock();
	auto nextSqLabel = findLabel("nextsq").lock();

	if (s.compare("nextsqvalue") == 0)
	{
		nextSqField->setTextPadded(sequencer.lock()->getNextSq() + 1, " ");
	}
	else if (s.compare("nextsq") == 0)
	{
		ls.lock()->setFunctionKeysArrangement(1);
		if (nextSqField->IsHidden()) {
			nextSqField->Hide(false);
			nextSqLabel->Hide(false);
			ls.lock()->setFocus("nextsq");
		}
		nextSqField->setTextPadded(sequencer.lock()->getNextSq() + 1, " ");
	}
	else if (s.compare("nextsqoff") == 0)
	{
		nextSqField->Hide(true);
		nextSqLabel->Hide(true);
		ls.lock()->setFunctionKeysArrangement(0);
		ls.lock()->setFocus("sq");
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
	}
	else if (s.compare("beat") == 0)
	{
		displayNow1();
	}
	else if (s.compare("clock") == 0)
	{
		displayNow2();
	}
	else if (s.compare("tempo") == 0)
	{
		displayTempo();
	}
	else if (s.compare("temposource") == 0)
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
	else if (s.compare("tracktype") == 0)
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
	else if (s.compare("soloenabled") == 0)
	{
		//soloLabel.lock()->setBlinking(sequencer.lock()->isSoloEnabled());
	}
}

void SequencerScreen::pressEnter()
{
	BaseControls::pressEnter();

	if (!isTypable())
	{
		return;
	}

	auto focusedField = findFocus().lock();
	auto focus = focusedField->getName();

	if (!focusedField->isTypeModeEnabled())
	{
		return;
	}

	auto candidate = focusedField->enter();

	if (candidate != INT_MAX) {	

		if (focus.compare("now0") == 0)
		{
			sequencer.lock()->setBar(candidate - 1);
			setLastFocus("step-editor", "viewmodenumber");
		}
		else if (focus.compare("now1") == 0)
		{
			sequencer.lock()->setBeat(candidate - 1);
			setLastFocus("step-editor", "viewmodenumber");
		}
		else if (focus.compare("now2") == 0)
		{
			sequencer.lock()->setClock(candidate);
			setLastFocus("step-editor", "viewmodenumber");
		}
		else if (focus.compare("tempo") == 0)
		{
			sequencer.lock()->setTempo(candidate * 0.1);
		}
		else if (focus.compare("velo") == 0)
		{
			track.lock()->setVelocityRatio(candidate);
		}
	}
}

void SequencerScreen::function(int i)
{
	init();
	BaseControls::function(i);

	switch (i)
	{
	case 0:
		openScreen("step-editor");
		break;
	case 1:
	{
		auto eventsScreen = dynamic_pointer_cast<EventsScreen>(Screens::getScreenComponent("events"));
		eventsScreen->setTime0(0);
		eventsScreen->setTime1(sequence.lock()->getLastTick());
		openScreen("events");
		break;
	}
	case 2:
		track.lock()->setOn(!track.lock()->isOn());
		break;
	case 3:
		sequencer.lock()->setSoloEnabled(!sequencer.lock()->isSoloEnabled());
		break;
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

	auto focus = findFocus().lock()->getName();

	if (focus.size() >= 3 && focus.substr(0, 3).compare("now") == 0)
	{
		setLastFocus("step-editor", "view");
	}

	if (focus.compare("now0") == 0)
	{
		sequencer.lock()->setBar(sequencer.lock()->getCurrentBarIndex() + i);
	}
	else if (focus.compare("now1") == 0)
	{
		sequencer.lock()->setBeat(sequencer.lock()->getCurrentBeatIndex() + i);
	}
	else if (focus.compare("now2") == 0)
	{
		sequencer.lock()->setClock(sequencer.lock()->getCurrentClockNumber() + i);
	}
	else if (focus.compare("devicenumber") == 0)
	{
		checkTrackUsed();
		track.lock()->setDeviceNumber(track.lock()->getDevice() + i);
	}
	else if (focus.compare("tr") == 0)
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
	else if (focus.compare("tracktype") == 0)
	{
		checkTrackUsed();

		track.lock()->setBusNumber(track.lock()->getBusNumber() + i);
		
		auto lastFocus = getLastFocus("step-editor");

		if (lastFocus.length() == 2)
		{
			auto eventNumber = stoi(lastFocus.substr(1, 2));

			auto stepEditorScreen = dynamic_pointer_cast<StepEditorScreen>(Screens::getScreenComponent("step-editor"));

			if (dynamic_pointer_cast<mpc::sequencer::NoteEvent>(stepEditorScreen->getVisibleEvents()[eventNumber].lock()))
			{
				if (track.lock()->getBusNumber() == 0)
				{
					if (lastFocus[0] == 'd' || lastFocus[0] == 'e')
					{
						setLastFocus("step-editor", "a" + to_string(eventNumber));
					}
				}
			}
		}

		lastFocus = getLastFocus("events");

		string midinote = "midinote";
		string drumnote = "drumnote";

		if (lastFocus.compare("") != 0) {
			if (std::mismatch(midinote.begin(), midinote.end(), lastFocus.begin()).first == midinote.end())
			{
				if (track.lock()->getBusNumber() != 0)
				{
					setLastFocus("events", "drumnote");
				}
			}
			else if (std::mismatch(drumnote.begin(), drumnote.end(), lastFocus.begin()).first == drumnote.end())
			{
				if (track.lock()->getBusNumber() == 0)
				{
					setLastFocus("events", "midinote0");
				}
			}
		}
	}
	else if (focus.compare("pgm") == 0)
	{
		checkTrackUsed();
		track.lock()->setProgramChange(track.lock()->getProgramChange() + i);
	}
	else if (focus.compare("velo") == 0)
	{
		checkTrackUsed();
		track.lock()->setVelocityRatio(track.lock()->getVelocityRatio() + i);
	}
	else if (focus.compare("timing") == 0)
	{
		auto screen = dynamic_pointer_cast<TimingCorrectScreen>(Screens::getScreenComponent("timing-correct"));
		auto noteValue = screen->getNoteValue();
		screen->setNoteValue(noteValue + i);
		setLastFocus("timing-correct", "notevalue");
		displayTiming();
	}
	else if (focus.compare("sq") == 0)
	{
		if (sequencer.lock()->isPlaying())
		{
			sequencer.lock()->setNextSq(sequencer.lock()->getCurrentlyPlayingSequenceIndex() + i);
		}
		else
		{
			sequencer.lock()->setActiveSequenceIndex(sequencer.lock()->getActiveSequenceIndex() + i);
		}
	}
	else if (focus.compare("nextsq") == 0)
	{
		sequencer.lock()->setNextSq(sequencer.lock()->getNextSq() + i);
	}
	else if (focus.compare("bars") == 0)
	{
		openScreen("change-bars-2");
	}
	else if (focus.compare("tempo") == 0)
	{
		double oldTempo = sequencer.lock()->getTempo();
		double newTempo = oldTempo + (i * 0.1);
		sequencer.lock()->setTempo(newTempo);
	}
	else if (focus.compare("tsig") == 0)
	{
		openScreen("change-tsig");
	}
	else if (focus.compare("temposource") == 0)
	{
		sequencer.lock()->setTempoSourceSequence(i > 0);
	}
	else if (focus.compare("count") == 0)
	{
		sequencer.lock()->setCountEnabled(i > 0);
	}
	else if (focus.compare("loop") == 0)
	{
		sequence.lock()->setLoopEnabled(i > 0);
	}
	else if (focus.compare("recordingmode") == 0)
	{
		sequencer.lock()->setRecordingModeMulti(i > 0);
	}
	else if (focus.compare("on") == 0)
	{
		checkTrackUsed();
		track.lock()->setOn(i > 0);
	}
}

void SequencerScreen::openWindow()
{
	init();

	if (sequencer.lock()->isPlaying())
	{
		return;
	}

	auto focus = findFocus().lock()->getName();
	
	if (focus.compare("sq") == 0)
	{
		openScreen("sequence");
	}
	else if (focus.find("now") != string::npos)
	{
		openScreen("time-display");
	}
	else if (focus.find("tempo") != string::npos)
	{
		openScreen("tempo-change");
	}
	else if (focus.compare("timing") == 0)
	{
		openScreen("timing-correct");
	}
	else if (focus.compare("tsig") == 0)
	{
		openScreen("change-tsig");
	}
	else if (focus.compare("count") == 0)
	{
		openScreen("count-metronome");
	}
	else if (focus.compare("loop") == 0)
	{
		openScreen("loop-bars-window");
	}
	else if (focus.compare("tr") == 0)
	{
		openScreen("track");
	}
	else if (focus.compare("on") == 0)
	{
		openScreen("erase-all-off-tracks");
	}
	else if (focus.compare("pgm") == 0)
	{
		openScreen("transmit-program-changes");
	}
	else if (focus.compare("recordingmode") == 0)
	{
		openScreen("multi-recording-setup");
	}
	else if (focus.compare("tracktype") == 0)
	{
		openScreen("midi-input");
	}
	else if (focus.compare("devicenumber") == 0)
	{
		openScreen("midi-output");
	}
	else if (focus.compare("bars") == 0)
	{
		openScreen("change-bars");
	}
	else if (focus.compare("velo") == 0) {
		openScreen("edit-velocity");
	}
}

void SequencerScreen::left()
{
	init();

	if (sequencer.lock()->getNextSq() != -1) {
		return;
	}

	BaseControls::left();
}

void SequencerScreen::right()
{
	init();

	if (sequencer.lock()->getNextSq() != -1)
	{
		return;
	}

	if (!sequence.lock()->isUsed())
	{
		auto userScreen = dynamic_pointer_cast<UserScreen>(Screens::getScreenComponent("user"));
		sequence.lock()->init(userScreen->lastBar);
		int index = sequencer.lock()->getActiveSequenceIndex();
		string name = moduru::lang::StrUtil::trim(sequencer.lock()->getDefaultSequenceName()) + moduru::lang::StrUtil::padLeft(to_string(index + 1), "0", 2);
		sequence.lock()->setName(name);
		sequencer.lock()->setActiveSequenceIndex(sequencer.lock()->getActiveSequenceIndex());
	}

	BaseControls::right();
}

void SequencerScreen::up()
{
	init();

	if (sequencer.lock()->getNextSq() != -1) {
		return;
	}

	BaseControls::up();
}

void SequencerScreen::down()
{
	init();

	if (sequencer.lock()->getNextSq() != -1)
	{
		return;
	}

	if (!sequence.lock()->isUsed())
	{
		auto userScreen = dynamic_pointer_cast<UserScreen>(Screens::getScreenComponent("user"));
		sequence.lock()->init(userScreen->lastBar);
		int index = sequencer.lock()->getActiveSequenceIndex();
		string name = moduru::lang::StrUtil::trim(sequencer.lock()->getDefaultSequenceName()) + moduru::lang::StrUtil::padLeft(to_string(index + 1), "0", 2);
		sequence.lock()->setName(name);
		sequencer.lock()->setActiveSequenceIndex(sequencer.lock()->getActiveSequenceIndex());
	}
	BaseControls::down();
}
