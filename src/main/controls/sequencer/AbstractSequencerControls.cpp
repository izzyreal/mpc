#include "AbstractSequencerControls.hpp"

#include <Mpc.hpp>

#include <ui/Uis.hpp>
#include <ui/sampler/SamplerGui.hpp>
#include <ui/sequencer/BarCopyGui.hpp>
#include <ui/sequencer/EditSequenceGui.hpp>
#include <ui/sequencer/SongGui.hpp>
#include <ui/sequencer/StepEditorGui.hpp>
#include <ui/sequencer/TrMoveGui.hpp>
#include <ui/sequencer/window/SequencerWindowGui.hpp>
#include <sampler/Program.hpp>
#include <sequencer/Sequence.hpp>
#include <sequencer/Track.hpp>
#include <sequencer/SeqUtil.hpp>
#include <sequencer/Sequencer.hpp>

using namespace mpc::controls::sequencer;
using namespace mpc::sequencer;
using namespace std;

AbstractSequencerControls::AbstractSequencerControls()
	: BaseControls()
{
	swGui = Mpc::instance().getUis().lock()->getSequencerWindowGui();
	seGui = Mpc::instance().getUis().lock()->getStepEditorGui();
	songGui = Mpc::instance().getUis().lock()->getSongGui();
	barCopyGui = Mpc::instance().getUis().lock()->getBarCopyGui();
	editSequenceGui = Mpc::instance().getUis().lock()->getEditSequenceGui();
	trMoveGui = Mpc::instance().getUis().lock()->getTrMoveGui();
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
			auto samplerGui = mpc::Mpc::instance().getUis().lock()->getSamplerGui();
			auto note = samplerGui->getNote() + i;
			auto pad = program.lock()->getPadNumberFromNote(note);
			samplerGui->setPadAndNote(pad, samplerGui->getNote());
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
