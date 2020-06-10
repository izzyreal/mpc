#include "SongScreen.hpp"

#include <sequencer/Step.hpp>
#include <sequencer/Song.hpp>

#include <Util.hpp>

#include <thirdp/bcmath/bcmath_stl.h>

using namespace mpc::lcdgui::screens;
using namespace moduru::lang;
using namespace std;

SongScreen::SongScreen(const int layerIndex)
	: ScreenComponent("song", layerIndex)
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
	sequencer.lock()->addObserver(this);
	auto song = sequencer.lock()->getSong(selectedSongIndex).lock();
	song->addObserver(this);
}

void SongScreen::close()
{
	sequencer.lock()->deleteObserver(this);
	auto song = sequencer.lock()->getSong(selectedSongIndex).lock();
	song->deleteObserver(this);
}

void SongScreen::init()
{
	BaseControls::init();
}

void SongScreen::up()
{
	init();
	
	if (param.compare("step1") == 0 || param.compare("sequence1") == 0 || param.compare("reps1") == 0)
	{
		if (offset == -1)
		{
			return;
		}
	
		setOffset(offset - 1);
		sequencer.lock()->setActiveSequenceIndex(sequencer.lock()->getSongSequenceIndex());
		sequencer.lock()->setBar(0);
	}
	else {
		BaseControls::up();
	}
}

void SongScreen::left()
{
	init();
	if (param.compare("sequence1") == 0)
	{
		ls.lock()->setFocus("step1");
	}
	else if (param.compare("reps1") == 0)
	{
		ls.lock()->setFocus("sequence1");
	}
	else if (param.compare("step1") == 0)
	{
		ls.lock()->setFocus("loop");
	}
	else
	{
		BaseControls::left();
	}
}


void SongScreen::right()
{
	init();
	
	if (param.compare("sequence1") == 0)
	{
		ls.lock()->setFocus("reps1");
	}
	else if (param.compare("step1") == 0)
	{
		ls.lock()->setFocus("sequence1");
	}
	else if (param.compare("loop") == 0)
	{
		ls.lock()->setFocus("step1");
	}
	else
	{
		BaseControls::right();
	}
}

void SongScreen::openWindow()
{
	init();
	if (param.compare("loop") == 0)
	{
		ls.lock()->openScreen("loop-song-window");
	}
}

void SongScreen::down()
{
	init();
	auto song = sequencer.lock()->getSong(selectedSongIndex);
	if (param.compare("step1") == 0 || param.compare("sequence1") == 0 || param.compare("reps1") == 0)
	{	
		if (offset == song.lock()->getStepAmount() - 1)
		{
			return;
		}

		setOffset(offset + 1);
		sequencer.lock()->setActiveSequenceIndex(sequencer.lock()->getSongSequenceIndex());
		sequencer.lock()->setBar(0);
	}
	else {
		BaseControls::down();
	}
}

void SongScreen::turnWheel(int i)
{
	init();
	
	auto song = sequencer.lock()->getSong(selectedSongIndex).lock();
	
	if (param.find("sequence") != string::npos)
	{
		if (offset + 1 > song->getStepAmount() - 1) {
			return;
		}
		
		auto seq = song->getStep(offset + 1)->getSequence();
		auto up = sequencer.lock()->getFirstUsedSeqUp(seq + 1);
		song->getStep(offset + 1)->setSequence(i < 0 ? sequencer.lock()->getFirstUsedSeqDown(seq - 1) : up);
		sequencer.lock()->setActiveSequenceIndex(sequencer.lock()->getSongSequenceIndex());
		sequencer.lock()->setBar(0);
	}
	else if (param.find("reps") != string::npos) {
		if (offset + 1 > song->getStepAmount() - 1)
		{
			return;
		}
		song->getStep(offset + 1)->setRepeats(song->getStep(offset + 1)->getRepeats() + i);
	}
	else if (param.compare("song") == 0)
	{
		setSelectedSongIndex(selectedSongIndex + i);
		setOffset(-1);
		init();
		if (song->isUsed() && song->getStepAmount() != 0)
		{
			sequencer.lock()->setActiveSequenceIndex(song->getStep(0)->getSequence());
		}
	}
	else if (param.compare("tempo") == 0 && !sequencer.lock()->isTempoSourceSequenceEnabled())
	{
		sequencer.lock()->setTempo(BCMath(sequencer.lock()->getTempo().toDouble() + (i / 10.0)));
	}
	else if (param.compare("temposource") == 0)
	{
		sequencer.lock()->setTempoSourceSequence(i > 0);
	}
	else if (param.compare("loop") == 0)
	{
		setLoop(i > 0);
	}
}

void SongScreen::function(int i)
{
	init();
	auto song = sequencer.lock()->getSong(selectedSongIndex).lock();
	
	switch (i)
	{
	case 4:
		song->deleteStep(offset + 1);
		break;
	case 5:
		song->insertStep(offset + 1, new mpc::sequencer::Step(song.get()));
		if (!song->isUsed())
		{
			song->setUsed(true);
		}
		break;
	}
}

void SongScreen::displayTempo()
{
	string tempo = sequencer.lock()->getTempo().toString();
	tempo = StrUtil::padLeft(tempo, " ", 5);
	tempo = mpc::Util::replaceDotWithSmallSpaceDot(tempo);
	findField("tempo").lock()->setText(tempo);
}

void SongScreen::displayLoop()
{
	findField("loop").lock()->setText(loop ? "YES" : "NO");
}

void SongScreen::displaySteps()
{
	auto song = sequencer.lock()->getSong(selectedSongIndex).lock();
	int steps = song->getStepAmount();
	auto s = sequencer.lock();
	
	auto stepArray = vector<weak_ptr<mpc::lcdgui::Field>>{ findField("step0"), findField("step1"), findField("step2") };
	auto sequenceArray = vector<weak_ptr<mpc::lcdgui::Field>>{ findField("sequence0"), findField("sequence1"), findField("sequence2") };
	auto repsArray = vector<weak_ptr<mpc::lcdgui::Field>>{ findField("reps0"), findField("reps1"), findField("reps2") };

	for (int i = 0; i < 3; i++)
	{
		int stepnr = i + offset;
	
		if (stepnr >= 0 && stepnr < steps)
		{
			stepArray[i].lock()->setTextPadded(stepnr + 1, " ");
			auto seqname = s->getSequence(song->getStep(stepnr)->getSequence()).lock()->getName();
			sequenceArray[i].lock()->setText(StrUtil::padLeft(to_string(song->getStep(stepnr)->getSequence() + 1), "0", 2) + "-" + seqname);
			repsArray[i].lock()->setText(to_string(song->getStep(stepnr)->getRepeats()));
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
	findField("temposource").lock()->setText(sequencer.lock()->isTempoSourceSequenceEnabled() ? "SEQ" : "MAS");
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
	auto song = sequencer.lock()->getSong(selectedSongIndex).lock();
	findField("song").lock()->setText(StrUtil::padLeft(to_string(selectedSongIndex + 1), "0", 2) + "-" + song->getName());
}

void SongScreen::setOffset(int i)
{
	if (i < -1)
	{
		return;
	}
	offset = i;
	displaySteps();
	displayTempo();
}

void SongScreen::setSelectedSongIndex(int i)
{
	if (i < 0 || i > 19)
	{
		return;
	}
	
	auto oldSong = sequencer.lock()->getSong(selectedSongIndex).lock();
	oldSong->deleteObserver(this);
	
	selectedSongIndex = i;

	auto newSong = sequencer.lock()->getSong(selectedSongIndex).lock();
	newSong->deleteObserver(this);

	displaySongName();
	displaySteps();
	displayTempoSource();
	displayTempo();
}

void SongScreen::setDefaultSongName(string s)
{
	defaultSongName = s;
}

void SongScreen::setLoop(bool b)
{
	loop = b;
	displayLoop();
}

void SongScreen::update(moduru::observer::Observable* observable, nonstd::any message)
{
	auto msg = nonstd::any_cast<string>(message);

	if (msg.compare("used") == 0 || msg.compare("song-name") == 0) {
		displaySongName();
	}
	else if (msg.compare("bar") == 0)
	{
		displayNow0();
	}
	else if (msg.compare("beat") == 0)
	{
		displayNow1();
	}
	else if (msg.compare("clock") == 0)
	{
		displayNow2();
	}
	else if (msg.compare("tempo") == 0) {
		displayTempo();
	}
	else if (msg.compare("step-editor") == 0) {
		displaySteps();
	}
	else if (msg.compare("play") == 0) {
		findField("sequence1").lock()->startBlinking();
		findField("reps1").lock()->startBlinking();
	}
	else if (msg.compare("stop") == 0) {
		findField("sequence1").lock()->stopBlinking();
		findField("reps1").lock()->stopBlinking();
	}
}

string SongScreen::getDefaultSongName()
{
	return defaultSongName;
}

int SongScreen::getOffset()
{
	return offset;
}

int SongScreen::getSelectedSongIndex()
{
	return selectedSongIndex;
}

bool SongScreen::isLoopEnabled()
{
	return loop;
}
