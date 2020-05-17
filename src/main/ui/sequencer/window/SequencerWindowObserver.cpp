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
	nameGui = Mpc::instance().getUis().lock()->getNameGui();
	samplerGui = Mpc::instance().getUis().lock()->getSamplerGui();
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
