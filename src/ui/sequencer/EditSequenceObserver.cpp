#include <ui/sequencer/EditSequenceObserver.hpp>
#include <Mpc.hpp>
#include <Util.hpp>
#include <ui/Uis.hpp>
#include <lcdgui/LayeredScreen.hpp>
#include <lcdgui/Field.hpp>
#include <lcdgui/Label.hpp>
#include <ui/sequencer/EditSequenceGui.hpp>
#include <sampler/Program.hpp>
#include <sampler/Sampler.hpp>
#include <sequencer/Sequence.hpp>
#include <sequencer/Track.hpp>
#include <sequencer/Sequencer.hpp>
#include <mpc/MpcSoundPlayerChannel.hpp>

#include <lang/StrUtil.hpp>

using namespace mpc::ui::sequencer;
using namespace std;

EditSequenceObserver::EditSequenceObserver(mpc::Mpc* mpc)
{
	sequencer = mpc->getSequencer();
	sampler = mpc->getSampler();
	functionNames = { "COPY", "DURATION", "VELOCITY", "TRANSPOSE" };
	modeNames = { "ADD VALUE", "SUB VALUE", "MULTI VAL%", "SET TO VAL" };
	editSequenceGui = mpc->getUis().lock()->getEditSequenceGui();
	editSequenceGui->addObserver(this);
	auto lSequencer = sequencer.lock();
	seqNum = lSequencer->getActiveSequenceIndex();
	sequence = lSequencer->getSequence(seqNum);
	trackNum = lSequencer->getActiveTrackIndex();
	auto seq = sequence.lock();
	track = seq->getTrack(trackNum);
	timeSig = seq->getTimeSignature();
	auto ls = mpc->getLayeredScreen().lock();
	editFunctionField = ls->lookupField("editfunction");
	editFunctionField.lock()->setSize(52 + 4, 9);
	time0Field = ls->lookupField("time0");
	time1Field = ls->lookupField("time1");
	time2Field = ls->lookupField("time2");
	time3Field = ls->lookupField("time3");
	time4Field = ls->lookupField("time4");
	time5Field = ls->lookupField("time5");
	drumNoteField = ls->lookupField("drumnote");
	midiNote0Field = ls->lookupField("midinote0");
	midiNote1Field = ls->lookupField("midinote1");
	midiNote1Label = ls->lookupLabel("midinote1");
	fromSqField = ls->lookupField("fromsq");
	tr0Field = ls->lookupField("tr0");
	toSqField = ls->lookupField("tosq");
	tr1Field = ls->lookupField("tr1");
	modeField = ls->lookupField("mode");
	start0Field = ls->lookupField("start0");
	start1Field = ls->lookupField("start1");
	start2Field = ls->lookupField("start2");
	copiesField = ls->lookupField("copies");
	fromSqLabel = ls->lookupLabel("fromsq");
	toSqLabel = ls->lookupLabel("tosq");
	tr0Label = ls->lookupLabel("tr0");
	tr1Label = ls->lookupLabel("tr1");
	start0Label = ls->lookupLabel("start0");
	start1Label = ls->lookupLabel("start1");
	start2Label = ls->lookupLabel("start2");
	copiesLabel = ls->lookupLabel("copies");
	copiesLabel.lock()->enableRigorousClearing();
	modeLabel = ls->lookupLabel("mode");
	modeLabel.lock()->enableRigorousClearing();
	lSequencer->deleteObservers();
	lSequencer->addObserver(this);
	seq->addObserver(this);
	auto lTrk = track.lock();
	lTrk->addObserver(this);
	timeSig.addObserver(this);
	setEditFunctionValue();
	displayTime();
	setNoteValues();
	fromSqField.lock()->setText(to_string(editSequenceGui->getFromSq() + 1));
	tr0Field.lock()->setText(to_string(editSequenceGui->getTr0() + 1));
	toSqField.lock()->setText(to_string(editSequenceGui->getToSq() + 1));
	tr1Field.lock()->setText(to_string(editSequenceGui->getTr1() + 1));
	setModeValue();
	displayStart();
	setCopiesValue();
}

void EditSequenceObserver::displayStart()
{
	auto seq = sequence.lock();
	start0Field.lock()->setText(moduru::lang::StrUtil().padLeft(to_string(EditSequenceGui::getBarNumber(seq.get(), editSequenceGui->getStartTicks()) + 1), "0", 3));
	start1Field.lock()->setText(moduru::lang::StrUtil().padLeft(to_string(EditSequenceGui::getBeatNumber(seq.get(), editSequenceGui->getStartTicks()) + 1), "0", 2));
	start2Field.lock()->setText(moduru::lang::StrUtil().padLeft(to_string(EditSequenceGui::getClockNumber(seq.get(), editSequenceGui->getStartTicks())), "0", 2));
}

void EditSequenceObserver::displayTime()
{
	auto seq = sequence.lock();
	time0Field.lock()->setText(moduru::lang::StrUtil().padLeft(to_string(EditSequenceGui::getBarNumber(seq.get(), editSequenceGui->getTime0()) + 1), "0", 3));
	time1Field.lock()->setText(moduru::lang::StrUtil().padLeft(to_string(EditSequenceGui::getBeatNumber(seq.get(), editSequenceGui->getTime0()) + 1), "0", 2));
	time2Field.lock()->setText(moduru::lang::StrUtil().padLeft(to_string(EditSequenceGui::getClockNumber(seq.get(), editSequenceGui->getTime0())), "0", 2));
	time3Field.lock()->setText(moduru::lang::StrUtil().padLeft(to_string(EditSequenceGui::getBarNumber(seq.get(), editSequenceGui->getTime1()) + 1), "0", 3));
	time4Field.lock()->setText(moduru::lang::StrUtil().padLeft(to_string(EditSequenceGui::getBeatNumber(seq.get(), editSequenceGui->getTime1()) + 1), "0", 2));
	time5Field.lock()->setText(moduru::lang::StrUtil().padLeft(to_string(EditSequenceGui::getClockNumber(seq.get(), editSequenceGui->getTime1())), "0", 2));
}

void EditSequenceObserver::setCopiesValue()
{
    if(editSequenceGui->getEditFunctionNumber() == 0) {
        copiesField.lock()->setText(moduru::lang::StrUtil::padLeft(to_string(editSequenceGui->getCopies()), " ", 3));
    }
    if(editSequenceGui->getEditFunctionNumber() == 1) {
        copiesField.lock()->setText(moduru::lang::StrUtil::padLeft(to_string(editSequenceGui->getDurationValue()), " ", 4));
    }
    if(editSequenceGui->getEditFunctionNumber() == 2) {
        copiesField.lock()->setText(moduru::lang::StrUtil::padLeft(to_string(editSequenceGui->getVelocityValue()), " ", 3));
    }
}

void EditSequenceObserver::setModeValue()
{
	if (editSequenceGui->getEditFunctionNumber() == 0) {
		modeField.lock()->setText(editSequenceGui->isModeMerge() ? "MERGE" : "REPLACE");
	}
	if (editSequenceGui->getEditFunctionNumber() == 1) {
		modeField.lock()->setText(modeNames[editSequenceGui->getDurationMode()]);
	}
	if (editSequenceGui->getEditFunctionNumber() == 2) {
		modeField.lock()->setText(modeNames[editSequenceGui->getVelocityMode()]);
	}
	if (editSequenceGui->getEditFunctionNumber() == 3) {
		if (editSequenceGui->getTransposeAmount() == 0) {
			modeField.lock()->setText("  0");
		}
		if (editSequenceGui->getTransposeAmount() < 0) {
			modeField.lock()->setText(moduru::lang::StrUtil::padLeft(to_string(editSequenceGui->getTransposeAmount()), " ", 3));
		}
		if (editSequenceGui->getTransposeAmount() > 0) {
			modeField.lock()->setText(moduru::lang::StrUtil::padLeft("+" + to_string(editSequenceGui->getTransposeAmount()), " ", 3));
		}
	}
}

void EditSequenceObserver::setEditFunctionValue()
{
    editFunctionField.lock()->setText(functionNames[editSequenceGui->getEditFunctionNumber()]);
    
	if (editSequenceGui->getEditFunctionNumber() == 0) {
        fromSqLabel.lock()->setLocation(132, 1);
        fromSqField.lock()->setLocation(fromSqField.lock()->getX(), 1);
        tr0Label.lock()->setLocation(tr0Label.lock()->getX(), 1);
        tr0Field.lock()->setLocation(tr0Field.lock()->getX(), 1);
        modeLabel.lock()->setText("Mode:");
        fromSqLabel.lock()->setText("From sq:");
        toSqField.lock()->Hide(false);
        tr1Field.lock()->Hide(false);
        start0Field.lock()->Hide(false);
        start1Field.lock()->Hide(false);
        start2Field.lock()->Hide(false);
        copiesField.lock()->Hide(false);
        toSqLabel.lock()->Hide(false);
        tr1Label.lock()->Hide(false);
        start0Label.lock()->Hide(false);
        start1Label.lock()->Hide(false);
        start2Label.lock()->Hide(false);
        copiesLabel.lock()->setText("Copies:");
		copiesLabel.lock()->setSize(7 * 6 + 1, 7);
        copiesLabel.lock()->setLocation(138, 39);
        copiesField.lock()->setLocation(copiesField.lock()->getX(), 38);
        modeLabel.lock()->setLocation(150, 20);
        modeField.lock()->setSize(7 * 6 + 1, 9);
        copiesField.lock()->setSize(3 * 6 + 1, 9);
    }
    else if (editSequenceGui->getEditFunctionNumber() == 1 || editSequenceGui->getEditFunctionNumber() == 2) {
        fromSqLabel.lock()->setLocation(132, 3);
        fromSqField.lock()->setLocation(fromSqField.lock()->getX(), 3);
        tr0Label.lock()->setLocation(tr0Label.lock()->getX(), 3);
        tr0Field.lock()->setLocation(tr0Field.lock()->getX(), 3);
        modeLabel.lock()->setText("Mode:");
        fromSqLabel.lock()->setText("Edit sq:");
        toSqField.lock()->Hide(true);
        tr1Field.lock()->Hide(true);
        start0Field.lock()->Hide(true);
        start1Field.lock()->Hide(true);
        start2Field.lock()->Hide(true);
        copiesField.lock()->Hide(false);
        toSqLabel.lock()->Hide(true);
        tr1Label.lock()->Hide(true);
        start0Label.lock()->Hide(true);
        start1Label.lock()->Hide(true);
        start2Label.lock()->Hide(true);
        copiesLabel.lock()->setText("Value:");
		copiesLabel.lock()->setSize(6 * 6 + 1, 7);
        copiesLabel.lock()->setLocation(138, 35);
        copiesField.lock()->setLocation(copiesField.lock()->getX(), 34);
        if(editSequenceGui->getEditFunctionNumber() == 2) {
            copiesField.lock()->setSize(3 * 6 + 1, 9);
        }
		else {
			copiesField.lock()->setSize(4 * 6 + 1, 9);
		}
        modeLabel.lock()->setLocation(150, 20);
        modeField.lock()->setSize(10 * 6 + 1, 9);
    }
    else if (editSequenceGui->getEditFunctionNumber() == 3) {
        fromSqLabel.lock()->setLocation(132, 3);
        fromSqField.lock()->setLocation(fromSqField.lock()->getX(), 3 );
        tr0Label.lock()->setLocation(tr0Label.lock()->getX(), 3);
        tr0Field.lock()->setLocation(tr0Field.lock()->getX(), 3);
        modeLabel.lock()->setText("Amount:");
        fromSqLabel.lock()->setText("Edit sq:");
        toSqField.lock()->Hide(true);
        tr1Field.lock()->Hide(true);
        start0Field.lock()->Hide(true);
        start1Field.lock()->Hide(true);
        start2Field.lock()->Hide(true);
        copiesField.lock()->Hide(true);
        toSqLabel.lock()->Hide(true);
        tr1Label.lock()->Hide(true);
        start0Label.lock()->Hide(true);
        start1Label.lock()->Hide(true);
        start2Label.lock()->Hide(true);
        copiesLabel.lock()->setText("(Except drum track)");
        copiesLabel.lock()->setLocation(132, 38);
		copiesLabel.lock()->setSize(112, 7);
		modeLabel.lock()->setLocation(138, 20);
        modeField.lock()->setSize(3 * 6 + 1, 9);
    }
}

void EditSequenceObserver::update(moduru::observer::Observable* o, std::any arg)
{
	auto lSequencer = sequencer.lock();
	auto lTrk = track.lock();
	lTrk->deleteObserver(this);
	sequence.lock()->deleteObserver(this);
	timeSig.deleteObserver(this);
	seqNum = lSequencer->getActiveSequenceIndex();
	sequence = lSequencer->getSequence(seqNum);
	trackNum = lSequencer->getActiveTrackIndex();
	auto seq = sequence.lock();
	track = seq->getTrack(trackNum);
	lTrk = track.lock();
	timeSig = seq->getTimeSignature();
	lTrk->addObserver(this);
	seq->addObserver(this);
	timeSig.addObserver(this);

	string s = std::any_cast<string>(arg);

	if (s.compare("modevalue") == 0) {
		setModeValue();
	}
	else if (s.compare("editfunction") == 0) {
		setEditFunctionValue();
		setModeValue();
		setCopiesValue();
	}
	else if (s.compare("time") == 0) {
		displayTime();
	}
	else if (s.compare("drumnote") == 0) {
		setDrumNoteValue();
	}
	else if (s.compare("midinote0") == 0) {
		setMidiNoteValues();
	}
	else if (s.compare("midinote1") == 0) {
		setMidiNoteValues();
	}
	else if (s.compare("fromsq") == 0) {
		fromSqField.lock()->setText(to_string(editSequenceGui->getFromSq() + 1));
	}
	else if (s.compare("tr0") == 0) {
		tr0Field.lock()->setText(to_string(editSequenceGui->getTr0() + 1));
	}
	else if (s.compare("tosq") == 0) {
		toSqField.lock()->setText(to_string(editSequenceGui->getToSq() + 1));
	}
	else if (s.compare("tr1") == 0) {
		tr1Field.lock()->setText(to_string(editSequenceGui->getTr1() + 1));
	}
	else if (s.compare("start") == 0) {
		displayStart();
	}
	else if (s.compare("copies") == 0) {
		setCopiesValue();
	}
}

void EditSequenceObserver::setNoteValues()
{
	auto lTrk = track.lock();
	midiNote0Field.lock()->setSize(8 * 6, 9);
	midiNote1Field.lock()->setSize(8 * 6, 9);
	midiNote1Field.lock()->setLocation(62, 39);
	if (lTrk->getBusNumber() == 0) {
		drumNoteField.lock()->Hide(true);
		midiNote0Field.lock()->Hide(false);
		midiNote1Field.lock()->Hide(false);
		midiNote1Label.lock()->Hide(false);
		setMidiNoteValues();
	}
	else {
		drumNoteField.lock()->Hide(false);
		midiNote0Field.lock()->Hide(true);
		midiNote1Field.lock()->Hide(true);
		midiNote1Label.lock()->Hide(true);
		setDrumNoteValue();
	}
}

void EditSequenceObserver::setMidiNoteValues()
{
	//midiNote0Field.lock()->setText(moduru::lang::StrUtil::padLeft(to_string(editSequenceGui->getMidiNote0()), " ", 3) + "(" + mpc::ui::Uis::noteNames[editSequenceGui->getMidiNote0()] + u8"\u00D4");
	//midiNote1Field.lock()->setText(moduru::lang::StrUtil::padLeft(to_string(editSequenceGui->getMidiNote1()), " ", 3) + "(" + mpc::ui::Uis::noteNames[editSequenceGui->getMidiNote1()] + u8"\u00D4");
}

void EditSequenceObserver::setDrumNoteValue()
{
	auto lTrk = track.lock();
	auto lSampler = sampler.lock();
	auto program = lSampler->getProgram(lSampler->getDrum(lTrk->getBusNumber() - 1)->getProgram());
	if (editSequenceGui->getDrumNote() == 34) {
		drumNoteField.lock()->setText("ALL");
	}
	else {
		drumNoteField.lock()->setText(moduru::lang::StrUtil::padLeft(to_string(editSequenceGui->getDrumNote()), " ", 2) + "/" + lSampler->getPadName(program.lock()->getPadNumberFromNote(editSequenceGui->getDrumNote())));
	}
}

EditSequenceObserver::~EditSequenceObserver() {
	editSequenceGui->deleteObserver(this);
	sequencer.lock()->deleteObserver(this);
	timeSig.deleteObserver(this);
	sequence.lock()->deleteObserver(this);
	track.lock()->deleteObserver(this);
}
