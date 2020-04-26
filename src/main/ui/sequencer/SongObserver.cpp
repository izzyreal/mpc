#include <ui/sequencer/SongObserver.hpp>

#include <Mpc.hpp>
#include <Util.hpp>
#include <ui/Uis.hpp>
#include <lcdgui/LayeredScreen.hpp>
#include <lcdgui/Field.hpp>
#include <lcdgui/Label.hpp>
#include <ui/sequencer/SongGui.hpp>
#include <sequencer/Sequence.hpp>
#include <sequencer/Sequencer.hpp>

#include <lang/StrUtil.hpp>

#include <sequencer/Step.hpp>
#include <sequencer/Song.hpp>
#include <thirdp/bcmath/bcmath_stl.h>

using namespace mpc::ui::sequencer;
using namespace std;

SongObserver::SongObserver(mpc::Mpc* mpc) 
{
	this->mpc = mpc;
	sequencer = mpc->getSequencer();
	auto lSequencer = sequencer.lock();
	lSequencer->deleteObservers();
	lSequencer->addObserver(this);
	songGui = mpc->getUis().lock()->getSongGui();
	songGui->deleteObservers();
	songGui->addObserver(this);
	song = lSequencer->getSong(songGui->getSelectedSongIndex());
	auto lSong = song.lock();
	lSong->deleteObservers();
	lSong->addObserver(this);
	auto ls = mpc->getLayeredScreen().lock();
	songField = ls->lookupField("song");
	now0Field = ls->lookupField("now0");
	now1Field = ls->lookupField("now1");
	now2Field = ls->lookupField("now2");
	tempoSourceField = ls->lookupField("temposource");
	tempoField = ls->lookupField("tempo");
	loopField = ls->lookupField("loop");
	step0Field = ls->lookupField("step0");
	step1Field = ls->lookupField("step1");
	step2Field = ls->lookupField("step2");
	sequence0Field = ls->lookupField("sequence0");
	sequence1Field = ls->lookupField("sequence1");
	sequence2Field = ls->lookupField("sequence2");
	reps0Field = ls->lookupField("reps0");
	reps1Field = ls->lookupField("reps1");
	reps2Field = ls->lookupField("reps2");

	step0Field.lock()->setFocusable(false);
	//step0Field.lock()->setOpaque(false);
	sequence0Field.lock()->setFocusable(false);
	//sequence0Field.lock()->setOpaque(false);
	reps0Field.lock()->setFocusable(false);
	//reps0Field.lock()->setOpaque(false);
	step2Field.lock()->setFocusable(false);
	//step2Field.lock()->setOpaque(false);
	sequence2Field.lock()->setFocusable(false);
	//sequence2Field.lock()->setOpaque(false);
	reps2Field.lock()->setFocusable(false);
	//reps2Field.lock()->setOpaque(false);

	displaySongName();
	displayNow0();
	displayNow1();
	displayNow2();
	displayTempoSource();
	displayTempo();
	displayLoop();
	displaySteps();
}

void SongObserver::displayTempo()
{
	string tempo = sequencer.lock()->getTempo().toString();
	tempo = moduru::lang::StrUtil::padLeft(tempo, " ", 5);
	tempo = Util::replaceDotWithSmallSpaceDot(tempo);
    tempoField.lock()->setText(tempo);
}

void SongObserver::displayLoop()
{
    loopField.lock()->setText(songGui->isLoopEnabled() ? "YES" : "NO");
}

void SongObserver::displaySteps()
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

void SongObserver::displayTempoSource()
{
    tempoSourceField.lock()->setText(sequencer.lock()->isTempoSourceSequenceEnabled() ? "SEQ" : "MAS");
}

void SongObserver::displayNow0()
{
	now0Field.lock()->setTextPadded(sequencer.lock()->getCurrentBarNumber() + 1, "0");
}

void SongObserver::displayNow1()
{
	now1Field.lock()->setTextPadded(sequencer.lock()->getCurrentBeatNumber() + 1, "0");
}

void SongObserver::displayNow2()
{
	now2Field.lock()->setTextPadded(sequencer.lock()->getCurrentClockNumber(), "0");
}

void SongObserver::displaySongName()
{
	songField.lock()->setText(moduru::lang::StrUtil::padLeft(to_string(songGui->getSelectedSongIndex() + 1), "0", 2) + "-" + song.lock()->getName());
}

void SongObserver::update(moduru::observer::Observable* o, nonstd::any arg)
{
	string s = nonstd::any_cast<string>(arg);
	if (s.compare("loop") == 0) {
		displayLoop();
	}
	else if (s.compare("song") == 0 || s.compare("selectedsongindex") == 0) {
		auto lSong = song.lock();
		if(lSong) lSong->deleteObservers();
		lSong = sequencer.lock()->getSong(songGui->getSelectedSongIndex()).lock();
		lSong->deleteObservers();
		lSong->addObserver(this);
		displaySongName();
		displaySteps();
		displayTempoSource();
		displayTempo();
	}
	else if (s.compare("used") == 0 || s.compare("songname") == 0) {
		displaySongName();
	}
	else if (s.compare("offset") == 0) {
		displaySteps();
		displayTempo();
	}
	else if (s.compare("temposource") == 0) {
		displayTempoSource();
	}
	else if (s.compare("tempo") == 0) {
		displayTempo();
	}
	else if (s.compare("step") == 0) {
		displaySteps();
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
	else if (s.compare("play") == 0) {
		sequence1Field.lock()->startBlinking();
		reps1Field.lock()->startBlinking();
	}
	else if (s.compare("stop") == 0) {
		sequence1Field.lock()->stopBlinking();
		reps1Field.lock()->stopBlinking();
	}
}

SongObserver::~SongObserver() {
	sequencer.lock()->deleteObserver(this);
	songGui->deleteObserver(this);
	song.lock()->deleteObserver(this);
}
