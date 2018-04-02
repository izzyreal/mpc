#include <ui/sequencer/window/SequencerWindowObserver.hpp>
#include <Mpc.hpp>
#include <Util.hpp>
#include <lcdgui/FunctionKeys.hpp>
#include <ui/Uis.hpp>
#include <lcdgui/LayeredScreen.hpp>
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
#include <ctootextensions/MpcSoundPlayerChannel.hpp>
#include <lcdgui/Field.hpp>
#include <lcdgui/Label.hpp>

#include <lang/StrUtil.hpp>

using namespace mpc::sequencer;
using namespace mpc::ui::sequencer::window;
using namespace std;

SequencerWindowObserver::SequencerWindowObserver(mpc::Mpc* mpc)
{
	this->mpc = mpc;
	softThruNames = {"OFF", "AS TRACK", "OMNI-A", "OMNI-B", "OMNI-AB" };
	editTypeNames = { "ADD VALUE", "SUB VALUE", "MULT VAL%", "SET TO VAL" };
	typeNames = { "NOTES", "PITCH BEND", "PROG CHANGE", "CH PRESSURE", "POLY PRESS", "EXCLUSIVE", "BANK SEL MSB", "MOD WHEEL", "BREATH CONT", "03", "FOOT CONTROL", "PORTA TIME", "DATA ENTRY", "MAIN VOLUME", "BALANCE", "09", "PAN", "EXPRESSION", "EFFECT 1"	, "EFFECT 2", "14", "15", "GEN.PUR. 1", "GEN.PUR. 2", "GEN.PUR. 3", "GEN.PUR. 4", "20", "21", "22", "23", "24", "25", "26", "27", "28", "29", "30", "31", "BANK SEL LSB", "MOD WHEL LSB", "BREATH LSB", "35", "FOOT CNT LSB", "PORT TIME LS", "DATA ENT LSB", "MAIN VOL LSB", "BALANCE LSB", "41", "PAN LSB", "EXPRESS LSB", "EFFECT 1 LSB", "EFFECT 2 MSB", "46", "47", "GEN.PUR.1 LS", "GEN.PUR.2 LS", "GEN.PUR.3 LS", "GEN.PUR.4 LS", "52", "53", "54", "55", "56", "57", "58", "59", "60", "61", "62", "63", "SUSTAIN PDL", "PORTA PEDAL", "SOSTENUTO", "SOFT PEDAL", "LEGATO FT SW", "HOLD 2", "SOUND VARI", "TIMBER/HARMO", "RELEASE TIME", "ATTACK TIME", "BRIGHTNESS", "SOUND CONT 6", "SOUND CONT 7", "SOUND CONT 8", "SOUND CONT 9", "SOUND CONT10", "GEN.PUR. 5", "GEN.PUR. 6", "GEN.PUR. 7", "GEN.PUR. 8", "PORTA CNTRL", "85", "86", "87", "88", "89", "90", "EXT EFF DPTH", "TREMOLO DPTH", "CHORUS DEPTH", " DETUNE DEPTH", "PHASER DEPTH", "DATA INCRE", "DATA DECRE", "NRPN LSB", "NRPN MSB", "RPN LSB", "RPN MSB", "102", "103", "104", "105", "106", "107" "108", "109", "110", "111", "112", "113", "114", "115", "116", "117", "118", "119", "ALL SND OFF", "RESET CONTRL", "LOCAL ON/OFF", "ALL NOTE OFF", "OMNI OFF", "OMNI ON", "MONO MODE ON", "POLY MODE ON" };
	noteValueNames = { "OFF", "1/8", "1/8(3)", "1/16", "1/16(3)", "1/32", "1/32(3)" };
	displayStyleNames = { "BAR,BEAT,CLOCK", "HOUR,MINUTE,SEC" };
	frameRateNames = { "24", "25", "30D", "30" };
	rateNames = { "1/4", "1/4(3)", "1/8", "1/8(3)", "1/16", "1/16(3)", "1/32", "1/32(3)" };
	countInNames = { "OFF", "REC ONLY", "REC+PLAY" };
	nameGui = mpc->getUis().lock()->getNameGui();
	samplerGui = mpc->getUis().lock()->getSamplerGui();
	inNames = vector<string>(34);
	for (int i = 0; i < 34; i++) {
		if (i < 16) {
			inNames[i] = "1-" + moduru::lang::StrUtil::padLeft(to_string(i + 1), "0", 2);
		}
		if (i == 16) {
			inNames[i] = "1-Ex";
		}
		if (i > 16 && i < 33) {
			inNames[i] = "2-" + moduru::lang::StrUtil::padLeft(to_string(i - 16), "0", 2);
		}
		if (i == 33) {
			inNames[i] = "2-Ex";
		}
	}
	sequencer = mpc->getSequencer();
	sampler = mpc->getSampler();
	auto ls = mpc->getLayeredScreen().lock();
	csn = ls->getCurrentScreenName();
	fb = ls->getFunctionKeys();
	hBars =ls->getHorizontalBarsTempoChangeEditor();
	swGui = mpc->getUis().lock()->getSequencerWindowGui();
	auto lSequencer = sequencer.lock();
	seqNum = lSequencer->getActiveSequenceIndex();
	sequence = lSequencer->getSequence(seqNum);
	trackNum = lSequencer->getActiveTrackIndex();
	auto seq = sequence.lock();
	track = seq->getTrack(trackNum);
	timeSig = seq->getTimeSignature();
	newTimeSignature = swGui->getNewTimeSignature();
	newTimeSignature->addObserver(this);
	auto lSampler = sampler.lock();
	int drum = track.lock()->getBusNumber() - 1;
	mpcSoundPlayerChannel = drum >= 0 ? lSampler->getDrum(drum) : nullptr;
	program = drum >= 0 ? lSampler->getProgram(mpcSoundPlayerChannel->getProgram()) : weak_ptr<mpc::sampler::Program>();
	sequenceNameFirstLetterField = ls->lookupField("sequencenamefirstletter");
	defaultSequenceNameFirstLetterField = ls->lookupField("defaultnamefirstletter");
	sequenceNameRestLabel = ls->lookupLabel("sequencenamerest");
	defaultSequenceNameRestLabel = ls->lookupLabel("defaultnamerest");
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
	displayStyleField = ls->lookupField("displaystyle");
	startTimeField = ls->lookupField("starttime");
	hField = ls->lookupField("h");
	mField = ls->lookupField("m");
	sField = ls->lookupField("s");
	fField = ls->lookupField("f");
	frameRateField = ls->lookupField("framerate");
	if (csn.compare("tempochange") == 0) {
		a0tcField = ls->lookupField("a0");
		a1tcField = ls->lookupField("a1");
		a2tcField = ls->lookupField("a2");
		b0tcField = ls->lookupField("b0");
		b1tcField = ls->lookupField("b1");
		b2tcField = ls->lookupField("b2");
		c0tcField = ls->lookupField("c0");
		c1tcField = ls->lookupField("c1");
		c2tcField = ls->lookupField("c2");
		d0tcField = ls->lookupField("d0");
		d1tcField = ls->lookupField("d1");
		d2tcField = ls->lookupField("d2");
		e0tcField = ls->lookupField("e0");
		e1tcField = ls->lookupField("e1");
		e2tcField = ls->lookupField("e2");
		f0tcField = ls->lookupField("f0");
		f1tcField = ls->lookupField("f1");
		f2tcField = ls->lookupField("f2");
		f0tcField.lock()->setSize(28, 9);
		f1tcField.lock()->setSize(28, 9);
		f2tcField.lock()->setSize(28, 9);
		tempoChangeField = ls->lookupField("tempochange");
		initialTempoField = ls->lookupField("initialtempo");
		initialTempoField.lock()->setSize(28, 9);
	}

	b1tcLabel = ls->lookupLabel("b1");
	c1tcLabel = ls->lookupLabel("c1");
	d1tcLabel = ls->lookupLabel("d1");
	e1tcLabel = ls->lookupLabel("e1");
	f1tcLabel = ls->lookupLabel("f1");
	b2tcLabel = ls->lookupLabel("b2");
	c2tcLabel = ls->lookupLabel("c2");
	d2tcLabel = ls->lookupLabel("d2");
	e2tcLabel = ls->lookupLabel("e2");
	f2tcLabel = ls->lookupLabel("f2");
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
	countInField = ls->lookupField("countin");
	inPlayField = ls->lookupField("inplay");
	rateField = ls->lookupField("rate");
	inRecField = ls->lookupField("inrec");
	waitForKeyField = ls->lookupField("waitforkey");
	firstBarField = ls->lookupField("firstbar");
	lastBarField = ls->lookupField("lastbar");
	numberOfBarsField = ls->lookupField("numberofbars");
	changeBarsAfterBarField = ls->lookupField("afterbar");
	changeBarsNumberOfBarsField = ls->lookupField("numberofbars");
	changeBarsFirstBarField = ls->lookupField("firstbar");
	changeBarsLastBarField = ls->lookupField("lastbar");
	inThisTrackField = ls->lookupField("inthistrack");
	newBarsField = ls->lookupField("newbars");
	message0Label = ls->lookupLabel("message0");
	message1Label = ls->lookupLabel("message1");
	currentLabel = ls->lookupLabel("current");
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
	lSequencer->addObserver(this);
	seq->addObserver(this);
	seq->getMetaTracks().at(2).lock()->addObserver(this);
	auto lTrk = track.lock();
	lTrk->addObserver(this);
	timeSig.addObserver(this);
	
	for (auto& h : hBars) {
		h.lock()->Hide(true);
	}

	if (csn.compare("sequence") == 0) {
		sequenceNameFirstLetterField.lock()->setText(seq->getName().substr(0, 1));
		defaultSequenceNameFirstLetterField.lock()->setText(lSequencer->getDefaultSequenceName().substr(0, 1));
		sequenceNameRestLabel.lock()->setText(seq->getName().substr(1, seq->getName().length()));
		defaultSequenceNameRestLabel.lock()->setText(lSequencer->getDefaultSequenceName().substr(1, lSequencer->getDefaultSequenceName().length()));
	}
	else if (csn.compare("track") == 0) {
		trackNameFirstLetterField.lock()->setText(lTrk->getName().substr(0, 1));
		defaultTrackNameFirstLetterField.lock()->setText(lSequencer->getDefaultTrackName(trackNum).substr(0, 1));
		trackNameRestLabel.lock()->setText(lTrk->getName().substr(1, lTrk->getName().length()));
		defaultTrackNameRestLabel.lock()->setText(lSequencer->getDefaultTrackName(trackNum).substr(1, lSequencer->getDefaultTrackName(trackNum).length()));
	}
	else if (csn.compare("deletesequence") == 0) {
		displaySequenceNumberName();
	}
	else if (csn.compare("deletetrack") == 0) {
		displayTrackNumber();
	}
	else if (csn.compare("copysequence") == 0) {
		displaySequenceNumberNames();
	}
	else if (csn.compare("copytrack") == 0) {
		displayTrackNumberNames();
	}
	else if (csn.compare("timedisplay") == 0) {
		displayDisplayStyle();
		displayStartTime();
		displayFrameRate();
	}
	else if (csn.compare("tempochange") == 0) {
		initVisibleEvents();
		displayTempoChange0();
		displayTempoChange1();
		displayTempoChange2();
		displayTempoChangeOn();
		displayInitialTempo();
	}
	else if (csn.compare("timingcorrect") == 0) {
		swGui->setTime1(seq->getLastTick());
		displayNoteValue();
		displaySwing();
		displayShiftTiming();
		displayAmount();
		displayTime();
		displayNotes();
	}
	else if (csn.compare("changetsig") == 0) {
		displayBars();
		displayNewTsig();
	}
	else if (csn.compare("countmetronome") == 0) {
		displayCountIn();
		displayInPlay();
		displayRate();
		displayInRec();
		displayWaitForKey();
	}
	else if (csn.compare("loopbarswindow") == 0) {
		displayFirstBar();
		displayLastBar();
		displayNumberOfBars();
	}
	else if (csn.compare("changebars") == 0) {
		displayChangeBarsAfterBar();
		displayChangeBarsNumberOfBars();
		displayChangeBarsFirstBar();
		displayChangeBarsLastBar();
	}
	else if (csn.compare("transmitprogramchanges") == 0) {
		displayTransmitProgramChangesInThisTrack();
	}
	else if (csn.compare("changebars2") == 0) {
		currentLabel.lock()->setText(to_string(seq->getLastBar() + 1));
		displayNewBars();
	}
	else if (csn.compare("multirecordingsetup") == 0) {
		for (auto& mrsLine : swGui->getMrsLines()) {
			mrsLine->setOut(seq->getTrack(mrsLine->getTrack()).lock()->getDevice());
		}
		swGui->setMrsYOffset(swGui->getMrsYOffset());
		displayMrsLine(0);
		displayMrsLine(1);
		displayMrsLine(2);
	}
	else if (csn.compare("midiinput") == 0) {
		displayReceiveCh();
		displayProgChangeSeq();
		displaySustainPedalToDuration();
		displayMidiFilter();
		displayType();
		displayPass();
	}
	else if (csn.compare("midioutput") == 0) {
		displaySoftThru();
		displayDeviceName();
	}
	else if (csn.compare("editvelocity") == 0) {
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

void SequencerWindowObserver::displayNewBars()
{
	auto seq = sequence.lock();
    newBarsField.lock()->setText(moduru::lang::StrUtil::padLeft(to_string(swGui->getNewBars() + 1), " ", 3));
    if(swGui->getNewBars() == seq->getLastBar()) {
        message0Label.lock()->setText("");
        message1Label.lock()->setText("");
    }
    if(swGui->getNewBars() > seq->getLastBar()) {
        message0Label.lock()->setText("Pressing DO IT will add");
        message1Label.lock()->setText("blank bars after last bar.");
    }
    if(swGui->getNewBars() < seq->getLastBar()) {
        message0Label.lock()->setText("Pressing DO IT will truncate");
        message1Label.lock()->setText("bars after last bar.");
    }
}

void SequencerWindowObserver::displayTransmitProgramChangesInThisTrack()
{
    inThisTrackField.lock()->setText(swGui->getTransmitProgramChangesInThisTrack() ? "YES" : "NO");
}

void SequencerWindowObserver::displayChangeBarsLastBar()
{
    changeBarsLastBarField.lock()->setText(to_string(swGui->getChangeBarsLastBar() + 1));
}

void SequencerWindowObserver::displayChangeBarsFirstBar()
{
    changeBarsFirstBarField.lock()->setText(to_string(swGui->getChangeBarsFirstBar() + 1));
}

void SequencerWindowObserver::displayChangeBarsNumberOfBars()
{
	changeBarsNumberOfBarsField.lock()->setText(to_string(swGui->getChangeBarsNumberOfBars()));
}

void SequencerWindowObserver::displayChangeBarsAfterBar()
{
	changeBarsAfterBarField.lock()->setText(to_string(swGui->getChangeBarsAfterBar()));
}

void SequencerWindowObserver::displayNumberOfBars()
{
	if (csn.compare("deletesequence") == 0) return;
	auto seq = sequence.lock();
	numberOfBarsField.lock()->setText(to_string(seq->getLastLoopBar() - seq->getFirstLoopBar() + 1));
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

void SequencerWindowObserver::displayFirstBar()
{
	auto seq = sequence.lock();
    firstBarField.lock()->setText(to_string(seq->getFirstLoopBar() + 1));
}

void SequencerWindowObserver::displayWaitForKey()
{
    waitForKeyField.lock()->setText(swGui->isWaitForKeyEnabled() ? "ON" : "OFF");
}

void SequencerWindowObserver::displayInRec()
{
    inRecField.lock()->setText(swGui->getInRec() ? "YES" : "NO");
}

void SequencerWindowObserver::displayRate()
{
    rateField.lock()->setText(rateNames[swGui->getRate()]);
}

void SequencerWindowObserver::displayInPlay()
{
    inPlayField.lock()->setText(swGui->getInPlay() ? "YES" : "NO");
}

void SequencerWindowObserver::displayCountIn()
{
    countInField.lock()->setText(countInNames[swGui->getCountInMode()]);
}

void SequencerWindowObserver::displayBars()
{
    bar0Field.lock()->setText(to_string(swGui->getBar0() + 1));
    bar1Field.lock()->setText(to_string(swGui->getBar1() + 1));
}

void SequencerWindowObserver::displayNewTsig()
{
	if (csn.compare("deletesequence") == 0) {
		return;
	}
	auto result = to_string(newTimeSignature->getNumerator()) + "/" + to_string(newTimeSignature->getDenominator());
	newTsigField.lock()->setText(mpc::Util::distributeTimeSig(result));
	//newTsigField.lock()->setText(result);
}

void SequencerWindowObserver::displayNoteValue()
{
    if(swGui->getNoteValue() != 0) {
        fb->enable(4);
    } else {
        fb->disable(4);
    }
    noteValueField.lock()->setText(noteValueNames[swGui->getNoteValue()]);
    if(swGui->getNoteValue() == 1 || swGui->getNoteValue() == 3) {
        swingLabel.lock()->Hide(false);
        swingField.lock()->Hide(false);
    } else {
        swingLabel.lock()->Hide(true);
        swingField.lock()->Hide(true);
    }
}

void SequencerWindowObserver::displaySwing()
{
    swingField.lock()->setText(to_string(swGui->getSwing()));
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
		if (swGui->getDrumNote() != 34) {
			notes0Field.lock()->setText(to_string(swGui->getDrumNote()) + "/" + lSampler->getPadName(program.lock()->getPadNumberFromNote(swGui->getDrumNote())));
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

void SequencerWindowObserver::displayShiftTiming()
{
    shiftTimingField.lock()->setText(swGui->isShiftTimingLater() ? "LATER" : "EARLIER");
}

void SequencerWindowObserver::displayAmount()
{
    amountField.lock()->setText(to_string(swGui->getAmount()));
}

void SequencerWindowObserver::initVisibleEvents()
{
	auto seq = sequence.lock();
	visibleTempoChangeEvents = vector<weak_ptr<mpc::sequencer::TempoChangeEvent>>(3);
	auto allTce = seq->getTempoChangeEvents();
	for (int i = 0; i < 3; i++) {
		if (i + swGui->getTempoChangeOffset() <allTce.size()) {
			visibleTempoChangeEvents[i] = allTce[i + swGui->getTempoChangeOffset()];
			auto tce = visibleTempoChangeEvents[i].lock();
			if (tce) {
				tce->addObserver(this);
			}
		}
		if (allTce.size() <= i + swGui->getTempoChangeOffset() + 1) {
			for (int j = i + 1; j < 2; j++) {
				visibleTempoChangeEvents[j] = weak_ptr<mpc::sequencer::TempoChangeEvent>();
			}
			break;
		}
	}
	swGui->setVisibleTempoChanges(visibleTempoChangeEvents);
}

void SequencerWindowObserver::displayInitialTempo()
{
	if (csn.compare("deletesequence") == 0) return;
	auto seq = sequence.lock();
	string tempoStr = seq->getInitialTempo().toString();
	tempoStr = Util::replaceDotWithSmallSpaceDot(tempoStr);
	initialTempoField.lock()->setText(tempoStr);
}

void SequencerWindowObserver::displayTempoChangeOn()
{
    tempoChangeField.lock()->setText(sequence.lock()->isTempoChangeOn() ? "YES" : "NO");
}

void SequencerWindowObserver::displayTempoChange0()
{
	hBars[1].lock()->Hide(false);
	auto tce = swGui->getVisibleTempoChanges()[0].lock();
	a0tcField.lock()->setText(" " + to_string(tce->getStepNumber() + 1));
	int value = tce->getBar(timeSig.getNumerator(), timeSig.getDenominator()) + 1;
	b0tcField.lock()->setTextPadded(value, "0");
	value = tce->getBeat(timeSig.getNumerator(), timeSig.getDenominator()) + 1;
	c0tcField.lock()->setTextPadded(value, "0");
	value = tce->getClock(timeSig.getNumerator(), timeSig.getDenominator());
	d0tcField.lock()->setTextPadded(value, "0");
	
	string ratioStr = moduru::lang::StrUtil::TrimDecimals(tce->getRatio() / 10.0, 1);
	ratioStr = moduru::lang::StrUtil::padLeft(ratioStr, " ", 5);
	ratioStr = Util::replaceDotWithSmallSpaceDot(ratioStr);
	e0tcField.lock()->setText(ratioStr);

	auto tempo = sequence.lock()->getInitialTempo().toDouble() * (tce->getRatio() / 1000.0);
	if (tempo < 30) {
		tempo = 30.0;
	}
	else if (tempo > 300) {
		tempo = 300.0;
	}
	string tempoStr = moduru::lang::StrUtil::TrimDecimals((tempo * 10) / 10.0, 1);
	tempoStr = moduru::lang::StrUtil::padLeft(tempoStr, " ", 5);
	tempoStr = Util::replaceDotWithSmallSpaceDot(tempoStr);
	f0tcField.lock()->setText(tempoStr);
	hBars[1].lock()->setValue((tempo - 15) * (290 / 975.0));
}

void SequencerWindowObserver::displayTempoChange1()
{
	int size = swGui->getVisibleTempoChanges().size();
	auto tce = swGui->getVisibleTempoChanges()[1].lock();
	if (!tce) {
		a1tcField.lock()->setText("END");
		b1tcField.lock()->Hide(true);
		c1tcField.lock()->Hide(true);
		d1tcField.lock()->Hide(true);
		e1tcField.lock()->Hide(true);
		f1tcField.lock()->Hide(true);
		b1tcLabel.lock()->Hide(true);
		c1tcLabel.lock()->Hide(true);
		d1tcLabel.lock()->Hide(true);
		e1tcLabel.lock()->Hide(true);
		f1tcLabel.lock()->Hide(true);
		hBars[2].lock()->Hide(true);
		return;
	}
	b1tcField.lock()->Hide(false);
	c1tcField.lock()->Hide(false);
	d1tcField.lock()->Hide(false);
	e1tcField.lock()->Hide(false);
	f1tcField.lock()->Hide(false);
	b1tcLabel.lock()->Hide(false);
	c1tcLabel.lock()->Hide(false);
	d1tcLabel.lock()->Hide(false);
	e1tcLabel.lock()->Hide(false);
	f1tcLabel.lock()->Hide(false);
	hBars[2].lock()->Hide(false);
	a1tcField.lock()->setText(" " + to_string(tce->getStepNumber() + 1));
	b1tcField.lock()->setTextPadded(tce->getBar(timeSig.getNumerator(), timeSig.getDenominator()) + 1, "0");
	c1tcField.lock()->setTextPadded(tce->getBeat(timeSig.getNumerator(), timeSig.getDenominator()) + 1, "0");
	d1tcField.lock()->setTextPadded(tce->getClock(timeSig.getNumerator(), timeSig.getDenominator()), "0");

	string ratioStr = moduru::lang::StrUtil::TrimDecimals(tce->getRatio() / 10.0, 1);
	ratioStr = moduru::lang::StrUtil::padLeft(ratioStr, " ", 5);
	ratioStr = Util::replaceDotWithSmallSpaceDot(ratioStr);
	e1tcField.lock()->setText(ratioStr);

	auto tempo = sequence.lock()->getInitialTempo().toDouble() * (tce->getRatio() / 1000.0);
	if (tempo < 30) {
		tempo = 30.0;
	}
	else if (tempo > 300) {
		tempo = 300.0;
	}
	string tempoStr = moduru::lang::StrUtil::TrimDecimals((tempo * 10) / 10.0, 1);
	tempoStr = moduru::lang::StrUtil::padLeft(tempoStr, " ", 5);
	tempoStr = Util::replaceDotWithSmallSpaceDot(tempoStr);
	f1tcField.lock()->setText(tempoStr);

	hBars[2].lock()->setValue((tempo - 15) * (290 / 975.0));
}

void SequencerWindowObserver::displayTempoChange2()
{
	auto tce = swGui->getVisibleTempoChanges()[2].lock();
	if (!tce) {
		if (!swGui->getVisibleTempoChanges()[1].lock()) {
			a2tcField.lock()->Hide(true);
		}
		else {
			a2tcField.lock()->Hide(false);
			a2tcField.lock()->setText("END");
		}
		b2tcField.lock()->Hide(true);
		c2tcField.lock()->Hide(true);
		d2tcField.lock()->Hide(true);
		e2tcField.lock()->Hide(true);
		f2tcField.lock()->Hide(true);
		b2tcLabel.lock()->Hide(true);
		c2tcLabel.lock()->Hide(true);
		d2tcLabel.lock()->Hide(true);
		e2tcLabel.lock()->Hide(true);
		f2tcLabel.lock()->Hide(true);
		hBars[3].lock()->Hide(true);
		return;
	}
	b2tcField.lock()->Hide(false);
	c2tcField.lock()->Hide(false);
	d2tcField.lock()->Hide(false);
	e2tcField.lock()->Hide(false);
	f2tcField.lock()->Hide(false);
	b2tcLabel.lock()->Hide(false);
	c2tcLabel.lock()->Hide(false);
	d2tcLabel.lock()->Hide(false);
	e2tcLabel.lock()->Hide(false);
	f2tcLabel.lock()->Hide(false);
	hBars[3].lock()->Hide(false);
	a2tcField.lock()->setText(" " + to_string(tce->getStepNumber() + 1));
	b2tcField.lock()->setTextPadded(tce->getBar(timeSig.getNumerator(), timeSig.getDenominator()) + 1, "0");
	c2tcField.lock()->setTextPadded(tce->getBeat(timeSig.getNumerator(), timeSig.getDenominator()) + 1, "0");
	d2tcField.lock()->setTextPadded(tce->getClock(timeSig.getNumerator(), timeSig.getDenominator()), "0");
	
	string ratioStr = moduru::lang::StrUtil::TrimDecimals(tce->getRatio() / 10.0, 1);
	ratioStr = moduru::lang::StrUtil::padLeft(ratioStr, " ", 5);
	ratioStr = Util::replaceDotWithSmallSpaceDot(ratioStr);
	e2tcField.lock()->setText(ratioStr);

	auto tempo = sequence.lock()->getInitialTempo().toDouble() * (tce->getRatio() / 1000.0);
	if (tempo < 30) {
		tempo = 30.0;
	}
	else if (tempo > 300) {
		tempo = 300.0;
	}
	string tempoStr = moduru::lang::StrUtil::TrimDecimals((tempo * 10) / 10.0, 1);
	tempoStr = moduru::lang::StrUtil::padLeft(tempoStr, " ", 5);
	tempoStr = Util::replaceDotWithSmallSpaceDot(tempoStr);
	f2tcField.lock()->setText(tempoStr);

	hBars[3].lock()->setValue((tempo - 15) * (290 / 975.0));
}

void SequencerWindowObserver::displayDisplayStyle()
{
    displayStyleField.lock()->setText(displayStyleNames[swGui->getDisplayStyle()]);
}

void SequencerWindowObserver::displayStartTime()
{
    startTimeField.lock()->setText(moduru::lang::StrUtil::padLeft(to_string(swGui->getStartTime()), "0", 2));
    hField.lock()->setText(moduru::lang::StrUtil::padLeft(to_string(swGui->getH()), "0", 2));
    mField.lock()->setText(moduru::lang::StrUtil::padLeft(to_string(swGui->getM()), "0", 2));
    sField.lock()->setText(moduru::lang::StrUtil::padLeft(to_string(swGui->getS()), "0", 2));
    fField.lock()->setText(moduru::lang::StrUtil::padLeft(to_string(swGui->getF()), "0", 2));
}

void SequencerWindowObserver::displayFrameRate()
{
    frameRateField.lock()->setText(frameRateNames[swGui->getFrameRate()]);
}

void SequencerWindowObserver::update(moduru::observer::Observable* o, boost::any arg)
{
	string s = boost::any_cast<string>(arg);
	auto seq = sequence.lock();
	auto lTrk = track.lock();
	lTrk->deleteObserver(this);
	seq->deleteObserver(this);
	timeSig.deleteObserver(this);
	auto lSequencer = sequencer.lock();
	seqNum = lSequencer->getActiveSequenceIndex();
	seq = lSequencer->getSequence(seqNum).lock();
	trackNum = lSequencer->getActiveTrackIndex();
	track = seq->getTrack(trackNum);
	lTrk = track.lock();
	timeSig = seq->getTimeSignature();
	lTrk->addObserver(this);
	seq->addObserver(this);
	timeSig.addObserver(this);
	int yPos;
	if (s.compare("initialtempo") == 0) {
		displayInitialTempo();
		initVisibleEvents();
		displayTempoChange0();
		displayTempoChange1();
		displayTempoChange2();
	}
	else if (s.compare("seqnumbername") == 0) {
		displaySequenceNumberName();
	}
	else if (s.compare("tracknumber") == 0) {
		displayTrackNumber();
	}
	else if (s.compare("tr0") == 0) {
		displayTrackNumberNames();
	}
	else if (s.compare("tr1") == 0) {
		displayTrackNumberNames();
	}
	else if (s.compare("sq0") == 0) {
		displaySequenceNumberNames();
	}
	else if (s.compare("sq1") == 0) {
		displaySequenceNumberNames();
	}
	else if (s.compare("starttime") == 0) {
		displayStartTime();
	}
	else if (s.compare("displaystyle") == 0) {
		displayDisplayStyle();
	}
	else if (s.compare("framerate") == 0) {
		displayFrameRate();
	}
	else if (s.compare("tempochange") == 0) {
		initVisibleEvents();
		//if (mpc->getLayeredScreen().lock()->getFocus().find("0") != string::npos) {
			displayTempoChange0();
		//}
		//else if (mpc->getLayeredScreen().lock()->getFocus().find("1") != string::npos) {
			displayTempoChange1();
		//}
		//else if (mpc->getLayeredScreen().lock()->getFocus().find("2") != string::npos) {
			displayTempoChange2();
		//}
	}
	else if (s.compare("offset") == 0 || s.compare("tempochangeadded") == 0 || s.compare("tick") == 0) {
		if (csn.compare("tempochange") == 0) {
			initVisibleEvents();
			displayTempoChange0();
			displayTempoChange1();
			displayTempoChange2();
		}
	}
	else if (s.compare("tempochangeon") == 0) {
		displayTempoChangeOn();
	}
	else if (s.compare("tempo") == 0) {
		displayInitialTempo();
	}
	else if (s.compare("time") == 0) {
		displayTime();
	}
	else if (s.compare("notevalue") == 0) {
		displayNoteValue();
	}
	else if (s.compare("swing") == 0) {
		displaySwing();
	}
	else if (s.compare("shifttiming") == 0) {
		displayShiftTiming();
	}
	else if (s.compare("amount") == 0) {
		displayAmount();
	}
	else if (s.compare("notes") == 0) {
		displayNotes();
	}
	else if (s.compare("bars") == 0) {
		displayBars();
	}
	else if (s.compare("timesignature") == 0) {
		displayNewTsig();
	}
	else if (s.compare("countin") == 0) {
		displayCountIn();
	}
	else if (s.compare("inplay") == 0) {
		displayInPlay();
	}
	else if (s.compare("rate") == 0) {
		displayRate();
	}
	else if (s.compare("inrec") == 0) {
		displayInRec();
	}
	else if (s.compare("waitforkey") == 0) {
		displayWaitForKey();
	}
	else if (s.compare("firstloopbar") == 0) {
		displayFirstBar();
		displayNumberOfBars();
	}
	else if (s.compare("lastloopbar") == 0) {
		displayLastBar();
		displayNumberOfBars();
	}
	else if (s.compare("numberofbars") == 0) {
		displayNumberOfBars();
		displayLastBar();
	}
	else if (s.compare("transmitprogramchangesinthistrack") == 0) {
		displayTransmitProgramChangesInThisTrack();
	}
	else if (s.compare("changebarsafterbar") == 0) {
		displayChangeBarsAfterBar();
	}
	else if (s.compare("changebarsnumberofbars") == 0) {
		displayChangeBarsNumberOfBars();
	}
	else if (s.compare("changebarsfirstbar") == 0) {
		displayChangeBarsFirstBar();
	}
	else if (s.compare("changebarslastbar") == 0) {
		displayChangeBarsLastBar();
	}
	else if (s.compare("newbars") == 0) {
		displayNewBars();
	}
	else if (s.compare("mrsline") == 0) {
		yPos = stoi(mpc->getLayeredScreen().lock()->getFocus().substr(1, 2));
		displayMrsLine(yPos);
	}
	else if (s.compare("multirecordingsetup") == 0) {
		displayMrsLine(0);
		displayMrsLine(1);
		displayMrsLine(2);
	}
	else if (s.compare("receivech") == 0) {
		displayReceiveCh();
	}
	else if (s.compare("progchangeseq") == 0) {
		displayProgChangeSeq();
	}
	else if (s.compare("sustainpedaltoduration") == 0) {
		displaySustainPedalToDuration();
	}
	else if (s.compare("midifilter") == 0) {
		displayMidiFilter();
	}
	else if (s.compare("type") == 0) {
		displayType();
	}
	else if (s.compare("pass") == 0) {
		displayPass();
	}
	else if (s.compare("softthru") == 0) {
		displaySoftThru();
	}
	else if (s.compare("devicenumber") == 0) {
		displayDeviceName();
	}
	else if (s.compare("edittype") == 0) {
		displayEditType();
	}
	else if (s.compare("value") == 0) {
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
	for (auto& t : visibleTempoChangeEvents) {
		if (t.lock()) t.lock()->deleteObservers();
	}
	for (auto& h : hBars) {
		h.lock()->Hide(true);
	}
	if (track.lock()) {
		track.lock()->deleteObserver(this);
	}
	sequencer.lock()->deleteObserver(this);
	newTimeSignature->deleteObserver(this);
	swGui->deleteObserver(this);
	nameGui->deleteObserver(this);
	if (sequence.lock()) {
		sequence.lock()->deleteObserver(this);
		sequence.lock()->getMetaTracks().at(2).lock()->deleteObserver(this);
	}
	timeSig.deleteObserver(this);
}
