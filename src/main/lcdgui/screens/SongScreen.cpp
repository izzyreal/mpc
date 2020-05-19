#include "SongScreen.hpp"

#include <lcdgui/LayeredScreen.hpp>
#include <ui/sequencer/SongGui.hpp>
#include <sequencer/Sequencer.hpp>
#include <sequencer/Step.hpp>
#include <sequencer/Song.hpp>

#include <thirdp/bcmath/bcmath_stl.h>

using namespace mpc::lcdgui::screens;
using namespace std;

SongScreen::SongScreen(const int& layer)
	: ScreenComponent("song", layer)
{
}

void SongScreen::open()
{
	displaySongName();
	displayNow0();
	displayNow1();
	displayNow2();
	displayTempoSource();
	displayTempo();
	displayLoop();
	displaySteps();
}

void SongScreen::init()
{
	super::init();
	step = songGui->getOffset() + 1;
	s = sequencer.lock()->getSong(songGui->getSelectedSongIndex());
}

void SongScreen::up()
{
	init();
	
	if (param.compare("step1") == 0 || param.compare("sequence1") == 0 || param.compare("reps1") == 0) {
		if (songGui->getOffset() == -1) return;
		songGui->setOffset(songGui->getOffset() - 1);
		sequencer.lock()->setActiveSequenceIndex(sequencer.lock()->getSongSequenceIndex());
		sequencer.lock()->setBar(0);
	}
	else {
		super::up();
	}
}

void SongScreen::left() {
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


void SongScreen::right() {
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

void SongScreen::openWindow()
{
	init();
	if (param.compare("loop") == 0) ls.lock()->openScreen("loopsongwindow");
}

void SongScreen::down()
{
	init();
	
	if (param.compare("step1") == 0 || param.compare("sequence1") == 0 || param.compare("reps1") == 0) {
		if (step == s.lock()->getStepAmount()) return;

		songGui->setOffset(songGui->getOffset() + 1);
		sequencer.lock()->setActiveSequenceIndex(sequencer.lock()->getSongSequenceIndex());
		sequencer.lock()->setBar(0);
	}
	else {
		super::down();
	}
}

void SongScreen::turnWheel(int i)
{
	init();
	
	auto lS = s.lock();
	if (param.find("sequence") != string::npos) {
		if (step > lS->getStepAmount() - 1) return;
		auto seq = lS->getStep(step)->getSequence();
		auto up = sequencer.lock()->getFirstUsedSeqUp(seq + 1);
		lS->getStep(step)->setSequence(i < 0 ? sequencer.lock()->getFirstUsedSeqDown(seq - 1) : up);
		sequencer.lock()->setActiveSequenceIndex(sequencer.lock()->getSongSequenceIndex());
		sequencer.lock()->setBar(0);
	}
	else if (param.find("reps") != string::npos) {
		if (step > lS->getStepAmount() - 1) return;
		lS->getStep(step)->setRepeats(lS->getStep(step)->getRepeats() + i);
	}
	else if (param.compare("song") == 0) {
		songGui->setSelectedSongIndex(songGui->getSelectedSongIndex() + i);
		songGui->setOffset(-1);
		init();
		if (lS->isUsed() && lS->getStepAmount() != 0)	sequencer.lock()->setActiveSequenceIndex(lS->getStep(0)->getSequence());
	}
	else if (param.compare("tempo") == 0 && !sequencer.lock()->isTempoSourceSequenceEnabled()) {
		sequencer.lock()->setTempo(BCMath(sequencer.lock()->getTempo().toDouble() + (i / 10.0)));
	}
	else if (param.compare("temposource") == 0) {
		sequencer.lock()->setTempoSourceSequence(i > 0);
	}
	else if (param.compare("loop") == 0) {
		songGui->setLoop(i > 0);
	}
}

void SongScreen::function(int i)
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

void SongScreen::displayTempo()
{
	string tempo = sequencer.lock()->getTempo().toString();
	tempo = moduru::lang::StrUtil::padLeft(tempo, " ", 5);
	tempo = Util::replaceDotWithSmallSpaceDot(tempo);
	findField("tempo").lock()->setText(tempo);
}

void SongScreen::displayLoop()
{
	findField("loop").lock()->setText(songGui->isLoopEnabled() ? "YES" : "NO");
}

void SongScreen::displaySteps()
{
	int offset = songGui->getOffset();
	auto lSong = song.lock();
	int steps = lSong->getStepAmount();
	auto s = sequencer.lock();
	auto stepArray = vector<weak_ptr<mpc::lcdgui::Field>>{ step0Field, step1Field, step2Field };
	auto sequenceArray = vector<weak_ptr<mpc::lcdgui::Field>>{ sequence0Field, sequence1Field, sequence2Field };
	auto repsArray = vector<weak_ptr<mpc::lcdgui::Field>>{ reps0Field, reps1Field, reps2Field };
	for (int i = 0; i < 3; i++) {
		int stepnr = i + offset;
		if (stepnr >= 0 && stepnr < steps) {
			stepArray[i].lock()->setTextPadded(stepnr + 1, " ");
			auto seqname = s->getSequence(lSong->getStep(stepnr)->getSequence()).lock()->getName();
			sequenceArray[i].lock()->setText(moduru::lang::StrUtil::padLeft(to_string(lSong->getStep(stepnr)->getSequence() + 1), "0", 2) + "-" + seqname);
			repsArray[i].lock()->setText(to_string(lSong->getStep(stepnr)->getRepeats()));
		}
		else {
			stepArray[i].lock()->setText("");
			sequenceArray[i].lock()->setText(stepnr == steps ? "   (end of song)" : "");
			repsArray[i].lock()->setText("");
		}
	}
}

void SongScreen::displayTempoSource()
{
	tempoSourceField.lock()->setText(sequencer.lock()->isTempoSourceSequenceEnabled() ? "SEQ" : "MAS");
}

void SongScreen::displayNow0()
{
	findField("now0").lock()->setTextPadded(sequencer.lock()->getCurrentBarNumber() + 1, "0");
}

void SongScreen::displayNow1()
{
	findField("now1").lock()->setTextPadded(sequencer.lock()->getCurrentBeatNumber() + 1, "0");
}

void SongScreen::displayNow2()
{
	findField("now2").lock()->setTextPadded(sequencer.lock()->getCurrentClockNumber(), "0");
}

void SongScreen::displaySongName()
{
	songField.lock()->setText(moduru::lang::StrUtil::padLeft(to_string(songGui->getSelectedSongIndex() + 1), "0", 2) + "-" + song.lock()->getName());
}
