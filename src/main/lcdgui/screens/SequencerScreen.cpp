#include "SequencerScreen.hpp"

#include <Mpc.hpp>
#include <sequencer/Sequencer.hpp>
#include <sequencer/Sequence.hpp>
#include <sequencer/Track.hpp>
#include <sequencer/TempoChangeEvent.hpp>
#include <sequencer/NoteEvent.hpp>

#include <sampler/Sampler.hpp>

#include <lcdgui/Parameter.hpp>
#include <lcdgui/Label.hpp>
#include <lcdgui/Field.hpp>

#include <ui/sequencer/window/SequencerWindowGui.hpp>
#include <ui/sequencer/EditSequenceGui.hpp>
#include <ui/sequencer/StepEditorGui.hpp>
#include <ui/UserDefaults.hpp>

#include <Util.hpp>

#include <lang/StrUtil.hpp>

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;

SequencerScreen::SequencerScreen(std::vector<std::shared_ptr<Component>> components)
	: ScreenComponent("sequencer"), sequencer(*mpc::Mpc::instance().getSequencer().lock().get()), sampler(*mpc::Mpc::instance().getSampler().lock().get())
{

	auto& mpc = mpc::Mpc::instance();

	sequence = mpc.getSequencer().lock()->getActiveSequence();
	track = mpc.getSequencer().lock()->getActiveTrack();

	for (auto component : components)
	{
		addChild(move(component));
	}

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

	mpc.getUis().lock()->getSequencerWindowGui()->addObserver(this);
	mpc.getSequencer().lock()->addObserver(this);

	sequence.lock()->addObserver(this);
	track.lock()->addObserver(this);
}

SequencerScreen::~SequencerScreen()
{
	auto& mpc = mpc::Mpc::instance();
	mpc.getSequencer().lock()->deleteObserver(this);
	mpc.getUis().lock()->getSequencerWindowGui()->deleteObserver(this);

	sequence.lock()->deleteObserver(this);
	track.lock()->deleteObserver(this);
}

void SequencerScreen::displayVelo()
{
	findField("velo").lock()->setText(to_string(sequencer.getActiveTrack().lock()->getVelocityRatio()));
}

void SequencerScreen::displayDeviceNumber()
{
	if (track.lock()->getDevice() == 0) {
		findField("devicenumber").lock()->setText("OFF");
	}
	else {
		if (track.lock()->getDevice() >= 17) {
			findField("devicenumber").lock()->setText(to_string(track.lock()->getDevice() - 16) + "B");
		}
		else {
			findField("devicenumber").lock()->setText(to_string(track.lock()->getDevice()) + "A");
		}
	}
}

vector<string> SequencerScreen::busNames = vector<string>{ "MIDI", "DRUM1", "DRUM2", "DRUM3", "DRUM4" };

void SequencerScreen::displayBus()
{
	findField("tracktype").lock()->setText(busNames[sequencer.getActiveTrack().lock()->getBusNumber()]);
	displayDeviceName();
}

void SequencerScreen::displayBars()
{
	findField("bars").lock()->setText(to_string(sequencer.getActiveSequence().lock()->getLastBar() + 1));
}

void SequencerScreen::displayPgm()
{
	if (track.lock()->getProgramChange() == 0) {
		findField("pgm").lock()->setText("OFF");
	}
	else {
		findField("pgm").lock()->setText(to_string(track.lock()->getProgramChange()));
	}
}

void SequencerScreen::displayDeviceName()
{
	if (track.lock()->getBusNumber() != 0) {
		if (track.lock()->getDevice() == 0) {
			int pgm = sampler.getDrumBusProgramNumber(track.lock()->getBusNumber());
			auto p = dynamic_pointer_cast<mpc::sampler::Program>(sampler.getProgram(pgm).lock());
			findLabel("devicename").lock()->setText(p->getName());
		}
		else {
			findLabel("devicename").lock()->setText(sequencer.getActiveSequence().lock()->getDeviceName(track.lock()->getDevice()));
		}
	}
	else if (track.lock()->getBusNumber() == 0) {
		if (track.lock()->getDevice() == 0) {
			findLabel("devicename").lock()->setText("NewPgm-A");
		}
		else {
			findLabel("devicename").lock()->setText(sequencer.getActiveSequence().lock()->getDeviceName(track.lock()->getDevice()));
		}
	}
}

void SequencerScreen::displayTempo()
{
	displayTempoLabel();
	auto tempo = sequencer.getTempo().toString();
	tempo = Util::replaceDotWithSmallSpaceDot(tempo);
	findField("tempo").lock()->setText(tempo);
}

void SequencerScreen::displayTempoLabel()
{
	auto currentRatio = -1;
	for (auto& tce : sequencer.getActiveSequence().lock()->getTempoChangeEvents()) {
		auto lTce = tce.lock();
		if (lTce->getTick() > sequencer.getTickPosition()) {
			break;
		}
		currentRatio = lTce->getRatio();
	}
	if (currentRatio != 1000) {
		findLabel("tempo").lock()->setText(u8"c\u00C0:");
	}
	else {
		findLabel("tempo").lock()->setText(u8" \u00C0:");
	}
}

void SequencerScreen::displayTempoSource()
{
	findField("temposource").lock()->setText(sequencer.isTempoSourceSequenceEnabled() ? "(SEQ)" : "(MAS)");
}


void SequencerScreen::displaySq()
{
	string result{ "" };

	if (sequencer.isPlaying()) {
		result.append(moduru::lang::StrUtil::padLeft(to_string(sequencer.getCurrentlyPlayingSequenceIndex() + 1), "0", 2));
		result.append("-");
		result.append(sequencer.getCurrentlyPlayingSequence().lock()->getName());
		findField("sq").lock()->setText(result);
	}
	else {
		result.append(moduru::lang::StrUtil::padLeft(to_string(sequencer.getActiveSequenceIndex() + 1), "0", 2));
		result.append("-");
		result.append(sequencer.getActiveSequence().lock()->getName());
		findField("sq").lock()->setText(result);
	}
}


void SequencerScreen::displayNow0()
{
	findField("now0").lock()->setTextPadded(sequencer.getCurrentBarNumber() + 1, "0");
}

void SequencerScreen::displayNow1()
{
	findField("now1").lock()->setTextPadded(sequencer.getCurrentBeatNumber() + 1, "0");
}

void SequencerScreen::displayNow2()
{
	findField("now2").lock()->setTextPadded(sequencer.getCurrentClockNumber(), "0");
}

void SequencerScreen::displayRecordingMode()
{
	findField("recordingmode").lock()->setText(sequencer.isRecordingModeMulti() ? "M" : "S");
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
	findField("on").lock()->setText(sequencer.getActiveTrack().lock()->isOn() ? "YES" : "NO");
}

void SequencerScreen::displayTr() {
	
	string result = moduru::lang::StrUtil::padLeft(to_string(sequencer.getActiveTrackIndex() + 1), "0", 2);
	result.append("-");
	result.append(sequencer.getActiveTrack().lock()->getName());
	findField("tr").lock()->setText(result);
}

void SequencerScreen::displayCount()
{
	findField("count").lock()->setText(sequencer.isCountEnabled() ? "ON" : "OFF");
}

vector<string> SequencerScreen::timingCorrectNames = vector<string>{ "OFF", "1/8", "1/8(3)", "1/16", "1/16(3)", "1/32", "1/32(3)" };

void SequencerScreen::displayTiming()
{
	findField("timing").lock()->setText(timingCorrectNames[mpc::Mpc::instance().getUis().lock()->getSequencerWindowGui()->getNoteValue()]);
}

void SequencerScreen::update(moduru::observer::Observable* o, nonstd::any arg)
{
	track.lock()->deleteObserver(this);
	sequence.lock()->deleteObserver(this);
	
	sequence = sequencer.getActiveSequence();
	track = sequencer.getActiveTrack();

	sequence.lock()->addObserver(this);
	track.lock()->addObserver(this);

	string s = nonstd::any_cast<string>(arg);

	auto nextSqField = findField("nextsq").lock();
	auto nextSqLabel = findLabel("nextsq").lock();



	if (s.compare("nextsqvalue") == 0)
	{
		nextSqField->setTextPadded(sequencer.getNextSq() + 1, " ");
	}
	else if (s.compare("nextsq") == 0)
	{
		//ls->drawFunctionKeys("nextsq");
		if (nextSqField->IsHidden()) {
			nextSqField->Hide(false);
			nextSqLabel->Hide(false);
			//ls->setFocus("nextsq");
		}
		nextSqField->setTextPadded(sequencer.getNextSq() + 1, " ");
	}
	else if (s.compare("nextsqoff") == 0)
	{
		nextSqField->Hide(true);
		nextSqLabel->Hide(true);
		//ls->drawFunctionKeys("sequencer");
		//ls->setFocus("sq");
	}
	else if (s.compare("notevalue") == 0)
	{
		displayTiming();
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
		//soloLabel.lock()->setBlinking(sequencer.isSoloEnabled());
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
			sequencer.setBar(candidate - 1);
			setLastFocus("sequencer_step", "viewmodenumber");
		}
		else if (focus.compare("now1") == 0)
		{
			sequencer.setBeat(candidate - 1);
			setLastFocus("sequencer_step", "viewmodenumber");
		}
		else if (focus.compare("now2") == 0)
		{
			sequencer.setClock(candidate);
			setLastFocus("sequencer_step", "viewmodenumber");
		}
		else if (focus.compare("tempo") == 0)
		{
			sequencer.setTempo(BCMath(candidate / 10.0));
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

	auto editSequenceGui = mpc::Mpc::instance().getUis().lock()->getEditSequenceGui();

	switch (i) {
	case 0:
		openScreen("sequencer_step");
		break;
	case 1:
		editSequenceGui->setTime1(sequence.lock()->getLastTick());
		openScreen("edit");
		break;
	case 2:
		track.lock()->setOn(!track.lock()->isOn());
		break;
	case 3:
		sequencer.setSoloEnabled(!sequencer.isSoloEnabled());
		break;
	case 4:
		sequencer.trackDown();
		break;
	case 5:
		sequencer.trackUp();
		break;
	}
}

void SequencerScreen::turnWheel(int i)
{
	init();

	auto focus = findFocus().lock()->getName();

	if (focus.size() >= 3 && focus.substr(0, 3).compare("now") == 0) {
		setLastFocus("sequencer_step", "viewmodenumber");
	}

	if (focus.compare("now0") == 0) {
		sequencer.setBar(sequencer.getCurrentBarNumber() + i);
	}
	else if (focus.compare("now1") == 0) {
		sequencer.setBeat(sequencer.getCurrentBeatNumber() + i);
	}
	else if (focus.compare("now2") == 0) {
		sequencer.setClock(sequencer.getCurrentClockNumber() + i);
	}
	else if (focus.compare("devicenumber") == 0) {
		track.lock()->setDeviceNumber(track.lock()->getDevice() + i);
	}
	else if (focus.compare("tr") == 0)
	{
		if (i > 0)
		{
			sequencer.trackUp();
		}
		else if (i < 0)
		{
			sequencer.trackDown();
		}
	}
	else if (focus.compare("tracktype") == 0)
	{
		track.lock()->setBusNumber(track.lock()->getBusNumber() + i);
		
		auto lastFocus = getLastFocus("sequencer_step");

		if (lastFocus.length() == 2)
		{
			auto eventNumber = stoi(lastFocus.substr(1, 2));

			auto stepEditorgui = mpc::Mpc::instance().getUis().lock()->getStepEditorGui();

			if (dynamic_pointer_cast<mpc::sequencer::NoteEvent>(stepEditorgui->getVisibleEvents()[eventNumber].lock())) {
				if (track.lock()->getBusNumber() == 0) {
					if (lastFocus[0] == 'd' || lastFocus[0] == 'e') {
						setLastFocus("sequencer_step", "a" + to_string(eventNumber));
					}
				}
			}
		}

		lastFocus = getLastFocus("edit");

		string midinote = "midinote";
		string drumnote = "drumnote";

		if (lastFocus.compare("") != 0) {
			if (std::mismatch(midinote.begin(), midinote.end(), lastFocus.begin()).first == midinote.end()) {
				if (track.lock()->getBusNumber() != 0) {
					setLastFocus("edit", "drumnote");
				}
			}
			else if (std::mismatch(drumnote.begin(), drumnote.end(), lastFocus.begin()).first == drumnote.end()) {
				if (track.lock()->getBusNumber() == 0) {
					setLastFocus("edit", "midinote0");
				}
			}
		}
	}
	else if (focus.compare("pgm") == 0) {
		track.lock()->setProgramChange(track.lock()->getProgramChange() + i);
	}
	else if (focus.compare("velo") == 0) {
		track.lock()->setVelocityRatio(track.lock()->getVelocityRatio() + i);
	}
	else if (focus.compare("timing") == 0) {
		auto sequencerWindowGui = mpc::Mpc::instance().getUis().lock()->getSequencerWindowGui();
		sequencerWindowGui->setNoteValue(sequencerWindowGui->getNoteValue() + i);
		setLastFocus("timingcorrect", "notevalue");
	}
	else if (focus.compare("sq") == 0) {
		if (sequencer.isPlaying()) {
			sequencer.setNextSq(sequencer.getCurrentlyPlayingSequenceIndex() + i);
		}
		else {
			sequencer.setActiveSequenceIndex(sequencer.getActiveSequenceIndex() + i);
		}
	}
	else if (focus.compare("nextsq") == 0) {
		sequencer.setNextSq(sequencer.getNextSq() + i);
	}
	else if (focus.compare("bars") == 0) {
		Mpc::instance().getUis().lock()->getSequencerWindowGui()->setNewBars(sequencer.getActiveSequence().lock()->getLastBar());
		openScreen("changebars2");
	}
	else if (focus.compare("tempo") == 0) {
		double oldTempo = sequencer.getTempo().toDouble();
		double newTempo = oldTempo + (i / 10.0);
		sequencer.setTempo(BCMath(newTempo));
	}
	else if (focus.compare("tsig") == 0) {
		mpc::sequencer::TimeSignature ts;
		auto oldTs = sequence.lock()->getTimeSignature();
		ts.setNumerator(oldTs.getNumerator());
		ts.setDenominator(oldTs.getDenominator());
		Mpc::instance().getUis().lock()->getSequencerWindowGui()->setNewTimeSignature(ts);
		openScreen("changetsig");
	}
	else if (focus.compare("temposource") == 0) {
		sequencer.setTempoSourceSequence(i > 0);
	}
	else if (focus.compare("count") == 0) {
		sequencer.setCountEnabled(i > 0);
	}
	else if (focus.compare("loop") == 0) {
		sequence.lock()->setLoopEnabled(i > 0);
	}
	else if (focus.compare("recordingmode") == 0) {
		sequencer.setRecordingModeMulti(i > 0);
	}
	else if (focus.compare("on") == 0) {
		track.lock()->setOn(i > 0);
	}
}

void SequencerScreen::openWindow()
{
	init();

	if (sequencer.isPlaying())
	{
		return;
	}

	auto focus = findFocus().lock()->getName();
	
	auto sequencerWindowGui = mpc::Mpc::instance().getUis().lock()->getSequencerWindowGui();

	if (focus.compare("sq") == 0)
	{
		openScreen("sequence");
	}
	else if (focus.find("now") != string::npos)
	{
		openScreen("timedisplay");
	}
	else if (focus.find("tempo") != string::npos)
	{
		openScreen("tempochange");
	}
	else if (focus.compare("timing") == 0)
	{
		openScreen("timingcorrect");
	}
	else if (focus.compare("tsig") == 0)
	{
		auto oldTs = sequence.lock()->getTimeSignature();
		auto newTs = Mpc::instance().getUis().lock()->getSequencerWindowGui()->getNewTimeSignature();
		newTs->setNumerator(oldTs.getNumerator());
		newTs->setDenominator(oldTs.getDenominator());
		openScreen("changetsig");
	}
	else if (focus.compare("count") == 0)
	{
		openScreen("countmetronome");
	}
	else if (focus.compare("loop") == 0)
	{
		openScreen("loopbarswindow");
	}
	else if (focus.compare("tr") == 0)
	{
		openScreen("track");
	}
	else if (focus.compare("on") == 0)
	{
		openScreen("eraseallofftracks");
	}
	else if (focus.compare("pgm") == 0)
	{
		openScreen("transmitprogramchanges");
	}
	else if (focus.compare("recordingmode") == 0)
	{
		openScreen("multirecordingsetup");
	}
	else if (focus.compare("tracktype") == 0)
	{
		openScreen("midiinput");
	}
	else if (focus.compare("devicenumber") == 0)
	{
		openScreen("midioutput");
	}
	else if (focus.compare("bars") == 0)
	{
		if (sequencerWindowGui->getChangeBarsAfterBar() > sequence.lock()->getLastBar() + 1)
		{
			sequencerWindowGui->setChangeBarsAfterBar(sequence.lock()->getLastBar() + 1, sequence.lock()->getLastBar());
		}
		
		if (sequencerWindowGui->getChangeBarsFirstBar() > sequence.lock()->getLastBar())
		{
			sequencerWindowGui->setChangeBarsFirstBar(sequence.lock()->getLastBar(), sequence.lock()->getLastBar());
		}

		if (sequencerWindowGui->getChangeBarsLastBar() > sequence.lock()->getLastBar())
		{
			sequencerWindowGui->setChangeBarsLastBar(sequence.lock()->getLastBar(), sequence.lock()->getLastBar());
		}
		openScreen("changebars");
	}
	else if (focus.compare("velo") == 0) {
		Mpc::instance().getUis().lock()->getSequencerWindowGui()->setTime0(0);
		Mpc::instance().getUis().lock()->getSequencerWindowGui()->setTime1(sequence.lock()->getLastTick());
		openScreen("editvelocity");
	}
}

void SequencerScreen::left()
{
	init();

	if (sequencer.getNextSq() != -1) {
		return;
	}

	BaseControls::left();
}

void SequencerScreen::right()
{
	init();

	if (sequencer.getNextSq() != -1) {
		return;
	}

	if (!sequence.lock()->isUsed()) {
		sequence.lock()->init(mpc::ui::UserDefaults::instance().getLastBarIndex());
		int index = sequencer.getActiveSequenceIndex();
		string name = moduru::lang::StrUtil::trim(sequencer.getDefaultSequenceName()) + moduru::lang::StrUtil::padLeft(to_string(index + 1), "0", 2);
		sequence.lock()->setName(name);
		sequencer.setActiveSequenceIndex(sequencer.getActiveSequenceIndex());
	}
	BaseControls::right();
}

void SequencerScreen::up()
{
	init();

	if (sequencer.getNextSq() != -1) {
		return;
	}

	BaseControls::up();
}

void SequencerScreen::down()
{
	init();

	if (sequencer.getNextSq() != -1) {
		return;
	}

	if (!sequence.lock()->isUsed()) {
		sequence.lock()->init(mpc::ui::UserDefaults::instance().getLastBarIndex());
		int index = sequencer.getActiveSequenceIndex();
		string name = moduru::lang::StrUtil::trim(sequencer.getDefaultSequenceName()) + moduru::lang::StrUtil::padLeft(to_string(index + 1), "0", 2);
		sequence.lock()->setName(name);
		sequencer.setActiveSequenceIndex(sequencer.getActiveSequenceIndex());
	}
	BaseControls::down();
}
