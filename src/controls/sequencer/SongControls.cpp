#include "SongControls.hpp"

#include <lcdgui/LayeredScreen.hpp>
#include <ui/sequencer/SongGui.hpp>
#include <sequencer/Sequencer.hpp>
#include <sequencer/Step.hpp>
#include <sequencer/Song.hpp>

#include <thirdp/bcmath/bcmath_stl.h>

using namespace mpc::controls::sequencer;
using namespace std;

SongControls::SongControls(mpc::Mpc* mpc)
	: AbstractSequencerControls(mpc)
{
}

void SongControls::init()
{
	super::init();
	step = songGui->getOffset() + 1;
	s = sequencer.lock()->getSong(songGui->getSelectedSongIndex());
}

void SongControls::up()
{
	init();
	auto lSequencer = sequencer.lock();
	if (param.compare("step1") == 0 || param.compare("sequence1") == 0 || param.compare("reps1") == 0) {
		if (songGui->getOffset() == -1) return;
		songGui->setOffset(songGui->getOffset() - 1);
		lSequencer->setActiveSequenceIndex(lSequencer->getSongSequenceIndex());
		lSequencer->setBar(0);
	}
	else {
		super::up();
	}
}

void SongControls::left() {
	init();
	if (param.compare("sequence1") == 0) {
		ls.lock()->setFocus("step1");
	}
	else if (param.compare("reps1") == 0) {
		ls.lock()->setFocus("sequence1");
	}
	else if (param.compare("step1") == 0) {
		ls.lock()->setFocus("loop");
	}
	else {
		super::left();
	}
}


void SongControls::right() {
	init();
	if (param.compare("sequence1") == 0) {
		ls.lock()->setFocus("reps1");
	}
	else if (param.compare("step1") == 0) {
		ls.lock()->setFocus("sequence1");
	}
	else if (param.compare("loop") == 0) {
		ls.lock()->setFocus("step1");
	}
	else {
		super::right();
	}
}

void SongControls::openWindow()
{
	init();
	if (param.compare("loop") == 0) ls.lock()->openScreen("loopsongwindow");
}

void SongControls::down()
{
	init();
	auto lSequencer = sequencer.lock();
	if (param.compare("step1") == 0 || param.compare("sequence1") == 0 || param.compare("reps1") == 0) {
		if (step == s.lock()->getStepAmount()) return;

		songGui->setOffset(songGui->getOffset() + 1);
		lSequencer->setActiveSequenceIndex(lSequencer->getSongSequenceIndex());
		lSequencer->setBar(0);
	}
	else {
		super::down();
	}
}

void SongControls::turnWheel(int i)
{
	init();
	auto lSequencer = sequencer.lock();
	auto lS = s.lock();
	if (param.find("sequence") != string::npos) {
		if (step > lS->getStepAmount() - 1) return;
		auto seq = lS->getStep(step)->getSequence();
		auto up = lSequencer->getFirstUsedSeqUp(seq + 1);
		lS->getStep(step)->setSequence(i < 0 ? lSequencer->getFirstUsedSeqDown(seq - 1) : up);
		lSequencer->setActiveSequenceIndex(lSequencer->getSongSequenceIndex());
		lSequencer->setBar(0);
	}
	else if (param.find("reps") != string::npos) {
		if (step > lS->getStepAmount() - 1) return;
		lS->getStep(step)->setRepeats(lS->getStep(step)->getRepeats() + i);
	}
	else if (param.compare("song") == 0) {
		songGui->setSelectedSongIndex(songGui->getSelectedSongIndex() + i);
		songGui->setOffset(-1);
		init();
		if (lS->isUsed() && lS->getStepAmount() != 0)	lSequencer->setActiveSequenceIndex(lS->getStep(0)->getSequence());
	}
	else if (param.compare("tempo") == 0 && !lSequencer->isTempoSourceSequence()) {
		lSequencer->setTempo(BCMath(lSequencer->getTempo().toDouble() + (i / 10.0)));
	}
	else if (param.compare("temposource") == 0) {
		lSequencer->setTempoSourceSequence(i > 0);
	}
	else if (param.compare("loop") == 0) {
		songGui->setLoop(i > 0);
	}
}

void SongControls::function(int i)
{
	init();
	auto lS = s.lock();
	switch (i) {
	case 4:
		lS->deleteStep(step);
		break;
	case 5:
		lS->insertStep(step, new mpc::sequencer::Step(lS.get()));
		if (!lS->isUsed()) lS->setUsed(true);
		break;
	}
}
