#include <ui/sequencer/SequencerObserver.hpp>
#include <lang/StrUtil.hpp>
#include <Mpc.hpp>
#include <ui/Uis.hpp>
#include <ui/sequencer/window/SequencerWindowGui.hpp>
#include <audiomidi/AudioMidiServices.hpp>
#include <lcdgui/LayeredScreen.hpp>
#include <lcdgui/Field.hpp>
#include <lcdgui/Label.hpp>
#include <sampler/Program.hpp>
#include <sampler/Sampler.hpp>
#include <sequencer/Sequence.hpp>
#include <sequencer/Track.hpp>
#include <sequencer/Sequencer.hpp>
#include <sequencer/TempoChangeEvent.hpp>
#include <sequencer/TimeSignature.hpp>
#include <Util.hpp>

#include <file/File.hpp>

using namespace mpc::ui::sequencer;
using namespace std;

SequencerObserver::SequencerObserver(mpc::Mpc* mpc)
{
	this->mpc = mpc;

	mpc->getUis().lock()->getSequencerWindowGui()->addObserver(this);

	ls = mpc->getLayeredScreen().lock().get();
	sequencer = mpc->getSequencer();
	auto lSequencer = sequencer.lock();
	lSequencer->addObserver(this);

	now0Field = ls->lookupField("now0");
	now1Field = ls->lookupField("now1");
	now2Field = ls->lookupField("now2");
	trField = ls->lookupField("tr");
	onField = ls->lookupField("on");
	sqField = ls->lookupField("sq");
	timingField = ls->lookupField("timing");
	countField = ls->lookupField("count");
	loopField = ls->lookupField("loop");
	recordingModeField = ls->lookupField("recordingmode");
	barsField = ls->lookupField("bars");
	tempoField = ls->lookupField("tempo");
	tempoField.lock()->setSize(28, 9);
	tempoSourceField = ls->lookupField("temposource");
	tsigField = ls->lookupField("tsig");
	pgmField = ls->lookupField("pgm");
	veloField = ls->lookupField("velo");
	busField = ls->lookupField("tracktype");
	deviceNumberField = ls->lookupField("devicenumber");

	deviceNameLabel = ls->lookupLabel("devicename");
	tempoLabel = ls->lookupLabel("tempo");
	nextSqField = ls->lookupField("nextsq");
	nextSqLabel = ls->lookupLabel("nextsq");

	tsigField.lock()->setColumns(7);

	nextSqField.lock()->Hide(true);
	nextSqLabel.lock()->Hide(true);

	soloLabel = dynamic_pointer_cast<mpc::lcdgui::BlinkLabel>(ls->lookupLabel("soloblink").lock());

	seqNum = lSequencer->getActiveSequenceIndex();
	seq = lSequencer->getSequence(seqNum);

	auto lSeq = seq.lock();
	lSeq->addObserver(this);

	busNames = vector<string>{ "MIDI", "DRUM1", "DRUM2", "DRUM3", "DRUM4" };
	sampler = mpc->getSampler();
	trackNum = lSequencer->getActiveTrackIndex();
	track = lSeq->getTrack(trackNum);
	auto lTrk = track.lock();
	lTrk->addObserver(this);

	displayTr();
	displayOn();
	displaySq();
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
	//soloLabel.lock()->setBlinking(lSequencer->isSoloEnabled());
}

vector<string>* SequencerObserver::timingCorrectNames()
{
	return &timingCorrectNames_;
}
vector<string> SequencerObserver::timingCorrectNames_ = vector<string>{ "OFF", "1/8", "1/8(3)", "1/16", "1/16(3)", "1/32", "1/32(3)" };

void SequencerObserver::displayTempoSource()
{
    tempoSourceField.lock()->setText(sequencer.lock()->isTempoSourceSequence() ? "(SEQ)" : "(MAS)");
}

void SequencerObserver::displaySq()
{
	string result{ "" };
	auto lSequencer = sequencer.lock();
    if(lSequencer->isPlaying()) {
		result.append(moduru::lang::StrUtil::padLeft(to_string(lSequencer->getCurrentlyPlayingSequenceIndex() + 1), "0", 2));
		result.append("-");
		result.append(lSequencer->getCurrentlyPlayingSequence().lock()->getName());
		sqField.lock()->setText(result);
    } else {
		result.append(moduru::lang::StrUtil::padLeft(to_string(lSequencer->getActiveSequenceIndex() + 1), "0", 2));
		result.append("-");
		result.append(lSequencer->getActiveSequence().lock()->getName());
		sqField.lock()->setText(result);
    }
}

void SequencerObserver::displayVelo()
{
    veloField.lock()->setText(to_string(track.lock()->getVelocityRatio()));
}

void SequencerObserver::displayDeviceNumber()
{
	auto lTrk = track.lock();
	if(lTrk->getDevice() == 0) {
        deviceNumberField.lock()->setText("OFF");
	}
	else {
		if (lTrk->getDevice() >= 17) {
			deviceNumberField.lock()->setText(to_string(lTrk->getDevice() - 16) + "B");
		}
		else {
			deviceNumberField.lock()->setText(to_string(lTrk->getDevice()) + "A");
		}
	}
}

void SequencerObserver::displayBus()
{
    busField.lock()->setText(busNames[track.lock()->getBusNumber()]);
    displayDeviceName();
}

void SequencerObserver::displayBars()
{
	barsField.lock()->setText(to_string((int)(seq.lock()->getLastBar() + 1)));
}

void SequencerObserver::displayPgm()
{
	auto lTrk = track.lock();
    if(lTrk->getProgramChange() == 0) {
        pgmField.lock()->setText("OFF");
    } else {
        pgmField.lock()->setText(to_string(lTrk->getProgramChange()));
    }
}

void SequencerObserver::displayDeviceName()
{
	auto lSampler = sampler.lock();
	auto lTrk = track.lock();
	if (lTrk->getBusNumber() != 0) {
		if (lTrk->getDevice() == 0) {
			int pgm = mpc->getAudioMidiServices().lock()->isDisabled() ? 0 : lSampler->getDrumBusProgramNumber(lTrk->getBusNumber());
			auto p = dynamic_pointer_cast<mpc::sampler::Program>(lSampler->getProgram(pgm).lock());
			deviceNameLabel.lock()->setText(p->getName());
		}
		else {
			deviceNameLabel.lock()->setText(seq.lock()->getDeviceName(lTrk->getDevice()));
		}
	}
	else if (lTrk->getBusNumber() == 0) {
		if (lTrk->getDevice() == 0) {
			deviceNameLabel.lock()->setText("NewPgm-A");
		}
		else {
			deviceNameLabel.lock()->setText(seq.lock()->getDeviceName(lTrk->getDevice()));
		}
	}
}

void SequencerObserver::displayTempo()
{
	displayTempoLabel();
	auto tempo = sequencer.lock()->getTempo().toString();
	tempo = Util::replaceDotWithSmallSpaceDot(tempo);
	tempoField.lock()->setText(tempo);
}

void SequencerObserver::displayTempoLabel()
{
	auto currentRatio = -1;
	for (auto& tce : seq.lock()->getTempoChangeEvents()) {
		auto lTce = tce.lock();
		if (lTce->getTick() > sequencer.lock()->getTickPosition()) {
			break;
		}
		currentRatio = lTce->getRatio();
	}
	if (currentRatio != 1000) {
		tempoLabel.lock()->setText(u8"c\u00C0:");
	}
	else {
		tempoLabel.lock()->setText(u8" \u00C0:");
	}
}

void SequencerObserver::update(moduru::observer::Observable* o, std::any arg)
{
    auto lSequencer = sequencer.lock();

	bool seqChanged = false;
	
	if (seqNum != lSequencer->getActiveSequenceIndex()) {
		seq.lock()->deleteObserver(this);
		seqNum = lSequencer->getActiveSequenceIndex();
		seq = lSequencer->getSequence(seqNum);
		seq.lock()->addObserver(this);
		seqChanged = true;
	}

	if (seqChanged || trackNum != lSequencer->getActiveTrackIndex()) {
		trackNum = lSequencer->getActiveTrackIndex();
		track.lock()->deleteObserver(this);
		track = seq.lock()->getTrack(trackNum);
		track.lock()->addObserver(this);
	}

	string s = std::any_cast<string>(arg);

	auto lNextSqField = nextSqField.lock();

	if (s.compare("nextsqvalue") == 0) {
		lNextSqField->setTextPadded(lSequencer->getNextSq() + 1, " ");
	}
	else if (s.compare("nextsq") == 0) {
		ls->drawFunctionKeys("nextsq");
		if(lNextSqField->IsHidden()) {
			lNextSqField->Hide(false);
			nextSqLabel.lock()->Hide(false);
			ls->setFocus("nextsq");
		}
		lNextSqField->setTextPadded(lSequencer->getNextSq() + 1, " ");
	}
	else if (s.compare("nextsqoff") == 0) {
		lNextSqField->Hide(true);
		nextSqLabel.lock()->Hide(true);
		ls->drawFunctionKeys("sequencer");
		ls->setFocus("sq");
	}
	else if (s.compare("notevalue") == 0) {
		displayTiming();
	}
	else if (s.compare("count") == 0) {
		displayCount();
	}
	else if (s.compare("tracknumbername") == 0) {
		displayTr();
		displayOn();
	}
	else if (s.compare("seqnumbername")==0) {
		displaySq();
	}
	else if (s.compare("loop")==0) {
		displayLoop();
    }
	else if (s.compare("recordingmode")==0) {
	   displayRecordingMode();
    }
	else if (s.compare("numberofbars") == 0) {
		displayBars();
	}
	else if (s.compare("trackon") == 0) {
		displayOn();
	}
	else if (s.compare("bar") == 0) {
		displayNow0();
	}
	else if (s.compare("beat") == 0) {
		displayNow1();
	}
	else if (s.compare("clock") == 0) {
		displayNow2();
	}
	else if (s.compare("tempo") == 0) {
		displayTempo();
	}
	else if (s.compare("temposource") == 0) {
		displayTempoSource();
	}
	else if (s.compare("timesignature") == 0) {
		displayTsig();
	}
	else if (s.compare("programchange") == 0) {
		displayPgm();
	}
	else if (s.compare("velocityratio") == 0) {
		displayVelo();
	}
	else if (s.compare("tracktype") == 0) {
		displayBus();
	}
	else if (s.compare("device") == 0) {
		displayDeviceNumber();
	}
	else if (s.compare("devicename") == 0) {
		displayDeviceName();
	}
	else if (s.compare("soloenabled") == 0) {
		soloLabel.lock()->setBlinking(lSequencer->isSoloEnabled());
	}
}

void SequencerObserver::displayNow0()
{
    now0Field.lock()->setTextPadded(sequencer.lock()->getCurrentBarNumber() + 1, "0");
}

void SequencerObserver::displayNow1()
{
    now1Field.lock()->setTextPadded(sequencer.lock()->getCurrentBeatNumber() + 1, "0");
}

void SequencerObserver::displayNow2()
{
    now2Field.lock()->setTextPadded(sequencer.lock()->getCurrentClockNumber(), "0");
}

void SequencerObserver::displayRecordingMode()
{
    recordingModeField.lock()->setText(sequencer.lock()->isRecordingModeMulti() ? "M" : "S");
}

void SequencerObserver::displayTsig()
{
	string result = "";
	auto lSeq = seq.lock();
	auto ts = lSeq->getTimeSignature();
	result.append(to_string(ts.getNumerator()));
	result.append("/");
	result.append(to_string(ts.getDenominator()));
	tsigField.lock()->setText(mpc::Util::distributeTimeSig(result));
}

void SequencerObserver::displayLoop()
{
	loopField.lock()->setText(seq.lock()->isLoopEnabled() ? "ON" : "OFF");
}

void SequencerObserver::displayOn() {
	onField.lock()->setText(track.lock()->isOn() ? "YES" : "NO");
}

void SequencerObserver::displayTr() {
	string result = moduru::lang::StrUtil::padLeft(to_string(trackNum + 1), "0", 2);
	result.append("-");
	result.append(seq.lock()->getTrack(trackNum).lock()->getName());
	trField.lock()->setText(result);
}

void SequencerObserver::displayCount() {
	countField.lock()->setText(sequencer.lock()->isCountEnabled() ? "ON" : "OFF");
}

void SequencerObserver::displayTiming() {
	timingField.lock()->setText(timingCorrectNames_[mpc->getUis().lock()->getSequencerWindowGui()->getNoteValue()]);
}

SequencerObserver::~SequencerObserver() {
	seq.lock()->deleteObserver(this);
	sequencer.lock()->deleteObserver(this);
	track.lock()->deleteObserver(this);
	mpc->getUis().lock()->getSequencerWindowGui()->deleteObserver(this);
}
