#include "SequencerWindowObserver.hpp"

#include <Mpc.hpp>
#include <Util.hpp>

#include <lcdgui/FunctionKeys.hpp>
#include <ui/Uis.hpp>
#include <lcdgui/LayeredScreen.hpp>
#include <ui/NameGui.hpp>
#include <lcdgui/HorizontalBar.hpp>
#include <lcdgui/Field.hpp>
#include <ui/sampler/SamplerGui.hpp>
#include <ui/sequencer/window/MultiRecordingSetupLine.hpp>
#include <ui/sequencer/window/SequencerWindowGui.hpp>
#include <sampler/Program.hpp>
#include <sampler/Sampler.hpp>
#include <sequencer/Event.hpp>
#include <sequencer/Sequence.hpp>
#include <sequencer/Track.hpp>
#include <sequencer/SeqUtil.hpp>
#include <sequencer/Sequencer.hpp>
#include <sequencer/TempoChangeEvent.hpp>
#include <mpc/MpcSoundPlayerChannel.hpp>
#include <lcdgui/Field.hpp>
#include <lcdgui/Label.hpp>

#include <lang/StrUtil.hpp>

using namespace mpc::sequencer;
using namespace mpc::ui::sequencer::window;
using namespace std;

SequencerWindowObserver::SequencerWindowObserver()
{
	
	softThruNames = {"OFF", "AS TRACK", "OMNI-A", "OMNI-B", "OMNI-AB" };
	editTypeNames = { "ADD VALUE", "SUB VALUE", "MULT VAL%", "SET TO VAL" };
	typeNames = { "NOTES", "PITCH BEND", "PROG CHANGE", "CH PRESSURE", "POLY PRESS", "EXCLUSIVE", "BANK SEL MSB", "MOD WHEEL", "BREATH CONT", "03", "FOOT CONTROL", "PORTA TIME", "DATA ENTRY", "MAIN VOLUME", "BALANCE", "09", "PAN", "EXPRESSION", "EFFECT 1"	, "EFFECT 2", "14", "15", "GEN.PUR. 1", "GEN.PUR. 2", "GEN.PUR. 3", "GEN.PUR. 4", "20", "21", "22", "23", "24", "25", "26", "27", "28", "29", "30", "31", "BANK SEL LSB", "MOD WHEL LSB", "BREATH LSB", "35", "FOOT CNT LSB", "PORT TIME LS", "DATA ENT LSB", "MAIN VOL LSB", "BALANCE LSB", "41", "PAN LSB", "EXPRESS LSB", "EFFECT 1 LSB", "EFFECT 2 MSB", "46", "47", "GEN.PUR.1 LS", "GEN.PUR.2 LS", "GEN.PUR.3 LS", "GEN.PUR.4 LS", "52", "53", "54", "55", "56", "57", "58", "59", "60", "61", "62", "63", "SUSTAIN PDL", "PORTA PEDAL", "SOSTENUTO", "SOFT PEDAL", "LEGATO FT SW", "HOLD 2", "SOUND VARI", "TIMBER/HARMO", "RELEASE TIME", "ATTACK TIME", "BRIGHTNESS", "SOUND CONT 6", "SOUND CONT 7", "SOUND CONT 8", "SOUND CONT 9", "SOUND CONT10", "GEN.PUR. 5", "GEN.PUR. 6", "GEN.PUR. 7", "GEN.PUR. 8", "PORTA CNTRL", "85", "86", "87", "88", "89", "90", "EXT EFF DPTH", "TREMOLO DPTH", "CHORUS DEPTH", " DETUNE DEPTH", "PHASER DEPTH", "DATA INCRE", "DATA DECRE", "NRPN LSB", "NRPN MSB", "RPN LSB", "RPN MSB", "102", "103", "104", "105", "106", "107" "108", "109", "110", "111", "112", "113", "114", "115", "116", "117", "118", "119", "ALL SND OFF", "RESET CONTRL", "LOCAL ON/OFF", "ALL NOTE OFF", "OMNI OFF", "OMNI ON", "MONO MODE ON", "POLY MODE ON" };
	nameGui = Mpc::instance().getUis().lock()->getNameGui();
	samplerGui = Mpc::instance().getUis().lock()->getSamplerGui();
	inNames = vector<string>(34);
	
	for (int i = 0; i < 34; i++)
	{
		if (i < 16)
		{
			inNames[i] = "1-" + moduru::lang::StrUtil::padLeft(to_string(i + 1), "0", 2);
		}
		
		if (i == 16)
		{
			inNames[i] = "1-Ex";
		}
		
		if (i > 16 && i < 33)
		{
			inNames[i] = "2-" + moduru::lang::StrUtil::padLeft(to_string(i - 16), "0", 2);
		}
		
		if (i == 33)
		{
			inNames[i] = "2-Ex";
		}
	}

	sequencer = Mpc::instance().getSequencer();
	sampler = Mpc::instance().getSampler();
	auto ls = Mpc::instance().getLayeredScreen().lock();
	csn = ls->getCurrentScreenName();
	fb = ls->getFunctionKeys();
	swGui = Mpc::instance().getUis().lock()->getSequencerWindowGui();
	auto lSequencer = sequencer.lock();
	seqNum = lSequencer->getActiveSequenceIndex();
	sequence = lSequencer->getSequence(seqNum);
	trackNum = lSequencer->getActiveTrackIndex();
	auto seq = sequence.lock();
	track = seq->getTrack(trackNum);

	auto lSampler = sampler.lock();
	int drum = track.lock()->getBusNumber() - 1;
	mpcSoundPlayerChannel = drum >= 0 ? lSampler->getDrum(drum) : nullptr;
	program = drum >= 0 ? dynamic_pointer_cast<mpc::sampler::Program>(lSampler->getProgram(mpcSoundPlayerChannel->getProgram()).lock()) : weak_ptr<mpc::sampler::Program>();
	trackNameFirstLetterField = ls->lookupField("tracknamefirstletter");
	defaultTrackNameFirstLetterField = ls->lookupField("defaultnamefirstletter");
	trackNameRestLabel = ls->lookupLabel("tracknamerest");
	defaultTrackNameRestLabel = ls->lookupLabel("defaultnamerest");
	trField = ls->lookupField("tr");
	sqField = ls->lookupField("sq");
	tr0Field = ls->lookupField("tr0");
	tr1Field = ls->lookupField("tr1");
	sq0Field = ls->lookupField("sq0");
	sq1Field = ls->lookupField("sq1");
	
	noteValueField = ls->lookupField("notevalue");
	swingField = ls->lookupField("swing");
	notes0Field = ls->lookupField("notes0");
	notes1Field = ls->lookupField("notes1");
	time0Field = ls->lookupField("time0");
	time1Field = ls->lookupField("time1");
	time2Field = ls->lookupField("time2");
	time3Field = ls->lookupField("time3");
	time4Field = ls->lookupField("time4");
	time5Field = ls->lookupField("time5");
	shiftTimingField = ls->lookupField("shifttiming");
	amountField = ls->lookupField("amount");
	swingLabel = ls->lookupLabel("swing");
	notes1Label = ls->lookupLabel("notes1");
	bar0Field = ls->lookupField("bar0");
	bar1Field = ls->lookupField("bar1");
	newTsigField = ls->lookupField("newtsig");
	firstBarField = ls->lookupField("firstbar");
	lastBarField = ls->lookupField("lastbar");
	numberOfBarsField = ls->lookupField("numberofbars");
	inThisTrackField = ls->lookupField("inthistrack");
	newBarsField = ls->lookupField("newbars");
	a0mrsField = ls->lookupField("a0");
	a1mrsField = ls->lookupField("a1");
	a2mrsField = ls->lookupField("a2");
	b0mrsField = ls->lookupField("b0");
	b1mrsField = ls->lookupField("b1");
	b2mrsField = ls->lookupField("b2");
	c0mrsField = ls->lookupField("c0");
	c1mrsField = ls->lookupField("c1");
	c2mrsField = ls->lookupField("c2");
	receiveChField = ls->lookupField("receivech");
	progChangeSeqField = ls->lookupField("seq");
	sustainPedalToDurationField = ls->lookupField("duration");
	midiFilterField = ls->lookupField("midifilter");
	typeField = ls->lookupField("type");
	passField = ls->lookupField("pass");
	softThruField = ls->lookupField("softthru");
	deviceNumberField = ls->lookupField("devicenumber");
	firstLetterField = ls->lookupField("firstletter");
	deviceNameLabel = ls->lookupLabel("devicename");
	editTypeField = ls->lookupField("edittype");
	valueField = ls->lookupField("value");
	
	swGui->addObserver(this);
	nameGui->addObserver(this);
	samplerGui->addObserver(this);
	lSequencer->addObserver(this);
	seq->addObserver(this);

	seq->getMetaTracks().at(2).lock()->addObserver(this);
	
	auto lTrk = track.lock();
	lTrk->addObserver(this);
	
	if (csn.compare("deletesequence") == 0)
	{
		displaySequenceNumberName();
	}
	else if (csn.compare("deletetrack") == 0)
	{
		displayTrackNumber();
	}
	else if (csn.compare("copysequence") == 0)
	{
		displaySequenceNumberNames();
	}
	else if (csn.compare("copytrack") == 0)
	{
		displayTrackNumberNames();
	}
	else if (csn.compare("loopbarswindow") == 0)
	{
		displayFirstBar();
		displayLastBar();
		displayNumberOfBars();
	}
	else if (csn.compare("transmitprogramchanges") == 0)
	{
		displayTransmitProgramChangesInThisTrack();
	}
	else if (csn.compare("multirecordingsetup") == 0)
	{
		for (auto& mrsLine : swGui->getMrsLines())
		{
			mrsLine->setOut(seq->getTrack(mrsLine->getTrack()).lock()->getDevice());
		}
		swGui->setMrsYOffset(swGui->getMrsYOffset());
		displayMrsLine(0);
		displayMrsLine(1);
		displayMrsLine(2);
	}
	else if (csn.compare("midiinput") == 0)
	{
		displayReceiveCh();
		displayProgChangeSeq();
		displaySustainPedalToDuration();
		displayMidiFilter();
		displayType();
		displayPass();
	}
	else if (csn.compare("midioutput") == 0)
	{
		displaySoftThru();
		displayDeviceName();
	}
	else if (csn.compare("editvelocity") == 0)
	{
		displayEditType();
		displayValue();
		displayTime();
		displayNotes();
	}
}

void SequencerWindowObserver::displaySoftThru()
{
    softThruField.lock()->setText(softThruNames[swGui->getSoftThru()]);
}

void SequencerWindowObserver::displayPass()
{
    passField.lock()->setText(swGui->getPass() ? "YES" : "NO");
}

void SequencerWindowObserver::displayType()
{
    typeField.lock()->setText(typeNames[swGui->getMidiFilterType()]);
}

void SequencerWindowObserver::displayMidiFilter()
{
    midiFilterField.lock()->setText(swGui->isMidiFilterEnabled() ? "ON" : "OFF");
}

void SequencerWindowObserver::displaySustainPedalToDuration()
{
    sustainPedalToDurationField.lock()->setText(swGui->isSustainPedalToDurationEnabled() ? "ON" : "OFF");
}

void SequencerWindowObserver::displayProgChangeSeq()
{
    progChangeSeqField.lock()->setText(swGui->getProgChangeSeq() ? "ON" : "OFF");
}

void SequencerWindowObserver::displayReceiveCh()
{
    if(swGui->getReceiveCh() == -1) {
        receiveChField.lock()->setText("ALL");
    } else {
        receiveChField.lock()->setText(to_string(swGui->getReceiveCh() + 1));
    }
}

void SequencerWindowObserver::displayMrsLine(int i)
{
	auto mrsLines = *swGui->getVisibleMrsLines();
	auto seq = sequence.lock();
	if (i == 0) {
		a0mrsField.lock()->setText(inNames[mrsLines[i]->getIn()]);
		if (mrsLines[i]->getTrack() == -1) {
			b0mrsField.lock()->setText("---OFF");
		}
		else {
			string trackNumber = to_string(mrsLines[i]->getTrack() + 1);
			trackNumber = moduru::lang::StrUtil::padLeft(trackNumber, "0", 2);
			b0mrsField.lock()->setText(string(trackNumber + "-" + seq->getTrack(mrsLines[i]->getTrack()).lock()->getName()));
		}
		if (mrsLines[i]->getOut() == 0) {
			c0mrsField.lock()->setText("OFF");
		}
		else {
			if (mrsLines[i]->getOut() >= 17) {
				string out = to_string(mrsLines[i]->getOut() - 16);
				c0mrsField.lock()->setTextPadded(string(out + "B"), " ");
			}
			else {
				string out = to_string(mrsLines[i]->getOut());
				c0mrsField.lock()->setTextPadded(string(out + "A"), " ");
			}
		}
	}
	else if (i == 1) {
		a1mrsField.lock()->setText(inNames[mrsLines[i]->getIn()]);
		if (mrsLines[i]->getTrack() == -1) {
			b1mrsField.lock()->setText("---OFF");
		}
		else {
			string trStr = moduru::lang::StrUtil::padLeft(to_string(mrsLines[i]->getTrack() + 1), "0", 2);
			b1mrsField.lock()->setText(string(trStr + "-" + seq->getTrack(mrsLines[i]->getTrack()).lock()->getName()));
		}
		if (mrsLines[i]->getOut() == 0) {
			c1mrsField.lock()->setText("OFF");
		}
		else {
			if (mrsLines[i]->getOut() >= 17) {
				c1mrsField.lock()->setTextPadded(to_string(mrsLines[i]->getOut() - 16) + "B", " ");
			}
			else {
				c1mrsField.lock()->setTextPadded(to_string(mrsLines[i]->getOut()) + "A", " ");
			}
		}
	}
	else if (i == 2) {
		a2mrsField.lock()->setText(inNames[mrsLines[i]->getIn()]);
		if (mrsLines[i]->getTrack() == -1) {
			b2mrsField.lock()->setText("---OFF");
		}
		else {
			b2mrsField.lock()->setText(moduru::lang::StrUtil::padLeft(to_string(mrsLines[i]->getTrack() + 1), "0", 2) + "-" + seq->getTrack(mrsLines[i]->getTrack()).lock()->getName());
		}
		if (mrsLines[i]->getOut() == 0) {
			c2mrsField.lock()->setText("OFF");
		}
		else {
			if (mrsLines[i]->getOut() >= 17) {
				c2mrsField.lock()->setTextPadded(to_string(mrsLines[i]->getOut() - 16) + "B", " ");
			}
			else {
				c2mrsField.lock()->setTextPadded(to_string(mrsLines[i]->getOut()) + "A", " ");
			}
		}
	}
}

void SequencerWindowObserver::displayLastBar()
{
    if(csn.compare("deletesequence") == 0) return;
	auto seq = sequence.lock();
    if(seq->isLastLoopBarEnd()) {
        lastBarField.lock()->setText("END");
    } else {
		lastBarField.lock()->setText(to_string(seq->getLastLoopBar() + 1));
    }
}

void SequencerWindowObserver::displayTransmitProgramChangesInThisTrack()
{
	inThisTrackField.lock()->setText(swGui->getTransmitProgramChangesInThisTrack() ? "YES" : "NO");
}

void SequencerWindowObserver::displayNumberOfBars()
{
	if (csn.compare("deletesequence") == 0) return;
	auto seq = sequence.lock();
	numberOfBarsField.lock()->setText(to_string(seq->getLastLoopBar() - seq->getFirstLoopBar() + 1));
}

void SequencerWindowObserver::displayFirstBar()
{
	auto seq = sequence.lock();
    firstBarField.lock()->setText(to_string(seq->getFirstLoopBar() + 1));
}

void SequencerWindowObserver::displayNotes()
{
	auto lSampler = sampler.lock();
	auto lTrk = track.lock();
	if (lTrk->getBusNumber() == 0) {
		notes0Field.lock()->setSize(8 * 6, 9);
		notes1Label.lock()->Hide(false);
		notes1Field.lock()->Hide(false);
		notes0Field.lock()->setText(moduru::lang::StrUtil::padLeft(to_string(swGui->getMidiNote0()), " ", 3) + "(" + mpc::ui::Uis::noteNames[swGui->getMidiNote0()] + u8"\u00D4");
		notes1Field.lock()->setText(moduru::lang::StrUtil::padLeft(to_string(swGui->getMidiNote1()), " ", 3) + "(" + mpc::ui::Uis::noteNames[swGui->getMidiNote1()] + u8"\u00D4");
	}
	else {
		notes0Field.lock()->setSize(6 * 6 + 2, 9);
		if (samplerGui->getNote() != 34) {
			notes0Field.lock()->setText(to_string(samplerGui->getNote()) + "/" + lSampler->getPadName(samplerGui->getPad()));
		}
		else {
			notes0Field.lock()->setText("ALL");
		}
		notes1Label.lock()->Hide(true);
		notes1Field.lock()->Hide(true);
	}
}

void SequencerWindowObserver::displayTime()
{
	auto s = sequence.lock().get();
	time0Field.lock()->setTextPadded(SeqUtil::getBarFromTick(s, swGui->getTime0()) + 1, "0");
	time1Field.lock()->setTextPadded(SeqUtil::getBeat(s, swGui->getTime0()) + 1, "0");
	time2Field.lock()->setTextPadded(SeqUtil::getClockNumber(s, swGui->getTime0()), "0");
	time3Field.lock()->setTextPadded(SeqUtil::getBarFromTick(s, swGui->getTime1()) + 1, "0");
	time4Field.lock()->setTextPadded(SeqUtil::getBeat(s, swGui->getTime1()) + 1, "0");
	time5Field.lock()->setTextPadded(SeqUtil::getClockNumber(s, swGui->getTime1()), "0");
}

void SequencerWindowObserver::update(moduru::observer::Observable* o, nonstd::any arg)
{
	string s = nonstd::any_cast<string>(arg);
	auto seq = sequence.lock();
	auto lTrk = track.lock();
	lTrk->deleteObserver(this);
	seq->deleteObserver(this);
	auto lSequencer = sequencer.lock();
	seqNum = lSequencer->getActiveSequenceIndex();
	seq = lSequencer->getSequence(seqNum).lock();
	trackNum = lSequencer->getActiveTrackIndex();
	track = seq->getTrack(trackNum);
	lTrk = track.lock();
	lTrk->addObserver(this);
	seq->addObserver(this);

	int yPos;

	if (s.compare("padandnote") == 0)
	{
		if (csn.compare("erase") == 0)
		{
			displayNotes();
		}
	}
	else if (s.compare("seqnumbername") == 0)
	{
		displaySequenceNumberName();
	}
	else if (s.compare("tracknumber") == 0)
	{
		displayTrackNumber();
	}
	else if (s.compare("tr0") == 0)
	{
		displayTrackNumberNames();
	}
	else if (s.compare("tr1") == 0)
	{
		displayTrackNumberNames();
	}
	else if (s.compare("sq0") == 0)
	{
		displaySequenceNumberNames();
	}
	else if (s.compare("sq1") == 0)
	{
		displaySequenceNumberNames();
	}
	else if (s.compare("time") == 0)
	{
		displayTime();
	}
	else if (s.compare("notes") == 0)
	{
		displayNotes();
	}
	else if (s.compare("firstloopbar") == 0)
{
		displayFirstBar();
		displayNumberOfBars();
	}
	else if (s.compare("lastloopbar") == 0)
{
		displayLastBar();
		displayNumberOfBars();
	}
	else if (s.compare("numberofbars") == 0)
{
		displayNumberOfBars();
		displayLastBar();
	}
	else if (s.compare("transmitprogramchangesinthistrack") == 0)
{
		displayTransmitProgramChangesInThisTrack();
	}
	else if (s.compare("mrsline") == 0)
	{
		yPos = stoi(Mpc::instance().getLayeredScreen().lock()->getFocus().substr(1, 2));
		displayMrsLine(yPos);
	}
	else if (s.compare("multirecordingsetup") == 0)
{
		displayMrsLine(0);
		displayMrsLine(1);
		displayMrsLine(2);
	}
	else if (s.compare("receivech") == 0)
{
		displayReceiveCh();
	}
	else if (s.compare("progchangeseq") == 0)
{
		displayProgChangeSeq();
	}
	else if (s.compare("sustainpedaltoduration") == 0)
{
		displaySustainPedalToDuration();
	}
	else if (s.compare("midifilter") == 0)
{
		displayMidiFilter();
	}
	else if (s.compare("type") == 0)
{
		displayType();
	}
	else if (s.compare("pass") == 0)
{
		displayPass();
	}
	else if (s.compare("softthru") == 0)
{
		displaySoftThru();
	}
	else if (s.compare("devicenumber") == 0)
{
		displayDeviceName();
	}
	else if (s.compare("edittype") == 0)
{
		displayEditType();
	}
	else if (s.compare("value") == 0)
	{
		displayValue();
	}
}

void SequencerWindowObserver::displayDeviceName()
{
    auto devName = sequence.lock()->getDeviceName(swGui->getDeviceNumber() + 1);
    firstLetterField.lock()->setText(devName.substr(0, 1));
    deviceNameLabel.lock()->setText(devName.substr(1, devName.length()));
	string devNumber = "";
    if(swGui->getDeviceNumber() >= 16) {
        devNumber = moduru::lang::StrUtil::padLeft(to_string(swGui->getDeviceNumber() - 15), " ", 2) + "B";
    } else {
		devNumber = moduru::lang::StrUtil::padLeft(to_string(swGui->getDeviceNumber() + 1), " ", 2) + "A";
	}
    deviceNumberField.lock()->setText(devNumber);
}

void SequencerWindowObserver::displayValue()
{
    valueField.lock()->setText(to_string(swGui->getValue()));
}

void SequencerWindowObserver::displayEditType()
{
    editTypeField.lock()->setText(editTypeNames[swGui->getEditType()]);
}

void SequencerWindowObserver::displayTrackNumber()
{
	trField.lock()->setText(moduru::lang::StrUtil::padLeft(to_string(swGui->getTrackNumber() + 1), " ", 2) + "-" +
		sequence.lock()->getTrack(swGui->getTrackNumber()).lock()->getName());
}

void SequencerWindowObserver::displaySequenceNumberName()
{
	auto lSequencer = sequencer.lock();
	auto sequenceName = lSequencer->getActiveSequence().lock()->getName();
    sqField.lock()->setText(moduru::lang::StrUtil::padLeft(to_string(lSequencer->getActiveSequenceIndex() + 1), " ", 2) + "-" + sequenceName);
}

void SequencerWindowObserver::displaySequenceNumberNames()
{
	auto lSequencer = sequencer.lock();
	auto sq0 = lSequencer->getSequence(swGui->getSq0()).lock()->getName();
    sq0Field.lock()->setText(moduru::lang::StrUtil::padLeft(to_string(swGui->getSq0() + 1), " ", 2) + "-" + sq0);
    auto sq1 = lSequencer->getSequence(swGui->getSq1()).lock()->getName();
    sq1Field.lock()->setText(moduru::lang::StrUtil::padLeft(to_string(swGui->getSq1() + 1), " ", 2) + "-" + sq1);
}

void SequencerWindowObserver::displayTrackNumberNames()
{
	auto seq = sequence.lock();
    auto tr0 = seq->getTrack(swGui->getTr0()).lock()->getName();
	tr0Field.lock()->setText(moduru::lang::StrUtil::padLeft(to_string(swGui->getTr0() + 1), " ", 2) + "-" + tr0);
    auto tr1 = seq->getTrack(swGui->getTr1()).lock()->getName();
    tr1Field.lock()->setText(moduru::lang::StrUtil::padLeft(to_string(swGui->getTr1() + 1), " ", 2) + "-" + tr1);
}

SequencerWindowObserver::~SequencerWindowObserver() {

	if (track.lock())
	{
		track.lock()->deleteObserver(this);
	}

	sequencer.lock()->deleteObserver(this);
	swGui->deleteObserver(this);
	nameGui->deleteObserver(this);
	samplerGui->deleteObserver(this);

	if (sequence.lock())
	{
		sequence.lock()->deleteObserver(this);
		sequence.lock()->getMetaTracks().at(2).lock()->deleteObserver(this);
	}
}
