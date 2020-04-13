#include "AbstractSequencerControls.hpp"

#include <Mpc.hpp>

#include <ui/Uis.hpp>
#include <ui/sequencer/BarCopyGui.hpp>
#include <ui/sequencer/EditSequenceGui.hpp>
#include <ui/sequencer/SongGui.hpp>
#include <ui/sequencer/StepEditorGui.hpp>
#include <ui/sequencer/TrMoveGui.hpp>
#include <ui/sequencer/window/SequencerWindowGui.hpp>
#include <sequencer/Sequence.hpp>
#include <sequencer/Track.hpp>
#include <sequencer/SeqUtil.hpp>
#include <sequencer/Sequencer.hpp>

using namespace mpc::controls::sequencer;
using namespace mpc::sequencer;
using namespace std;

AbstractSequencerControls::AbstractSequencerControls(mpc::Mpc* mpc)
	: BaseControls(mpc)
{
	swGui = mpc->getUis().lock()->getSequencerWindowGui();
	seGui = mpc->getUis().lock()->getStepEditorGui();
	songGui = mpc->getUis().lock()->getSongGui();
	barCopyGui = mpc->getUis().lock()->getBarCopyGui();
	editSequenceGui = mpc->getUis().lock()->getEditSequenceGui();
	trMoveGui = mpc->getUis().lock()->getTrMoveGui();
}

void AbstractSequencerControls::init()
{
	super::init();
	auto lSequencer = sequencer.lock();
	if (csn.compare("barcopy") == 0) {
		fromSeq = lSequencer->getSequence(barCopyGui->getFromSq());
		toSeq = lSequencer->getSequence(barCopyGui->getToSq());
	}
	else {
		fromSeq = lSequencer->getSequence(editSequenceGui->getFromSq());
		toSeq = lSequencer->getSequence(editSequenceGui->getToSq());
	}
	sequence = lSequencer->getActiveSequence();
}

void AbstractSequencerControls::checkAllTimesAndNotes(int i)
{
	init();
	auto s = sequence.lock();
	if (param.compare("time0") == 0) {
		swGui->setTime0(SeqUtil::getTickFromBar((SeqUtil::getBarFromTick(s.get(), swGui->getTime0())) + i, s.get(), swGui->getTime0()));
	}
	else if (param.compare("time1") == 0) {
		swGui->setTime0(SeqUtil::setBeat((SeqUtil::getBeat(s.get(), swGui->getTime0())) + i, s.get(), swGui->getTime0()));
	}
	else if (param.compare("time2") == 0) {
		swGui->setTime0(SeqUtil::setClockNumber((SeqUtil::getClockNumber(s.get(), swGui->getTime0())) + i, s.get(), swGui->getTime0()));
	}
	else if (param.compare("time3") == 0) {
		swGui->setTime1(SeqUtil::getTickFromBar((SeqUtil::getBarFromTick(s.get(), swGui->getTime1())) + i, s.get(), swGui->getTime1()));
	}
	else if (param.compare("time4") == 0) {
		swGui->setTime1(SeqUtil::setBeat((SeqUtil::getBeat(s.get(), swGui->getTime1())) + i, s.get(), swGui->getTime1()));
	}
	else if (param.compare("time5") == 0) {
		swGui->setTime1(SeqUtil::setClockNumber((SeqUtil::getClockNumber(s.get(), swGui->getTime1())) + i, s.get(), swGui->getTime1()));
	}
	else if (param.compare("notes0") == 0) {
		if (track.lock()->getBusNumber() != 0) {
			swGui->setDrumNote(swGui->getDrumNote() + i);
		}
		else {
			swGui->setMidiNote0(swGui->getMidiNote0() + i);
		}
	}
	else if (param.compare("notes1") == 0) {
		swGui->setMidiNote1(swGui->getMidiNote1() + i);
	}
}

AbstractSequencerControls::~AbstractSequencerControls() {
}
