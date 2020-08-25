#include "SongScreen.hpp"

#include <sequencer/Step.hpp>
#include <sequencer/Song.hpp>

#include <Util.hpp>

using namespace mpc::lcdgui::screens;
using namespace moduru::lang;
using namespace std;

SongScreen::SongScreen(mpc::Mpc& mpc, const int layerIndex)
	: ScreenComponent(mpc, "song", layerIndex)
{
}

void SongScreen::open()
{
	init();
	displaySongName();
	displayNow0();
	displayNow1();
	displayNow2();
	displayTempoSource();
	displayTempo();
	displayLoop();
	displaySteps();
	sequencer.lock()->addObserver(this);
}

void SongScreen::close()
{
	sequencer.lock()->deleteObserver(this);
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
		ScreenComponent::up();
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
		ScreenComponent::left();
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
		ScreenComponent::right();
	}
}

void SongScreen::openWindow()
{
	init();

	if (param.compare("loop") == 0)
		openScreen("loop-song-window");
	else if (param.compare("song") == 0)
		openScreen("song-window");
}

void SongScreen::down()
{
	init();
	auto song = sequencer.lock()->getSong(activeSongIndex);
	if (param.compare("step1") == 0 || param.compare("sequence1") == 0 || param.compare("reps1") == 0)
	{	
		if (offset == song.lock()->getStepCount() - 1)
			return;

		setOffset(offset + 1);
		sequencer.lock()->setActiveSequenceIndex(sequencer.lock()->getSongSequenceIndex());
		sequencer.lock()->setBar(0);
	}
	else
	{
		ScreenComponent::down();
	}
}

void SongScreen::turnWheel(int i)
{
	init();
	
	auto song = sequencer.lock()->getSong(activeSongIndex).lock();
	
	if (param.find("sequence") != string::npos)
	{
		if (offset + 1 > song->getStepCount() - 1)
			return;
		
		auto seq = song->getStep(offset + 1).lock()->getSequence();
		auto up = sequencer.lock()->getFirstUsedSeqUp(seq + 1);
		song->getStep(offset + 1).lock()->setSequence(i < 0 ? sequencer.lock()->getFirstUsedSeqDown(seq - 1) : up);
		sequencer.lock()->setActiveSequenceIndex(sequencer.lock()->getSongSequenceIndex());
		sequencer.lock()->setBar(0);
	}
	else if (param.find("reps") != string::npos)
	{
		if (offset + 1 > song->getStepCount() - 1)
			return;
	
		song->getStep(offset + 1).lock()->setRepeats(song->getStep(offset + 1).lock()->getRepeats() + i);
		displaySteps();
	}
	else if (param.compare("song") == 0)
	{
		setSelectedSongIndex(activeSongIndex + i);
		setOffset(-1);
		init();

		if (song->isUsed() && song->getStepCount() != 0)
			sequencer.lock()->setActiveSequenceIndex(song->getStep(0).lock()->getSequence());
	}
	else if (param.compare("tempo") == 0 && !sequencer.lock()->isTempoSourceSequenceEnabled())
	{
		sequencer.lock()->setTempo(sequencer.lock()->getTempo() + (i * 0.1));
	}
	else if (param.compare("temposource") == 0)
	{
		sequencer.lock()->setTempoSourceSequence(i > 0);
		displayTempoSource();
		displayTempo();
	}
	else if (param.compare("loop") == 0)
	{
		setLoop(i > 0);
	}
}

void SongScreen::function(int i)
{
	init();
	auto song = sequencer.lock()->getSong(activeSongIndex).lock();
	
	switch (i)
	{
	case 4:
		song->deleteStep(offset + 1);
		break;
	case 5:
		song->insertStep(offset + 1);
		
		if (!song->isUsed())
		{
			song->setUsed(true);
			auto newName = StrUtil::trim(defaultSongName) + StrUtil::padLeft(to_string(activeSongIndex + 1), "0", 2);
			song->setName(newName);
		}

		displaySongName();
		displaySteps();
		displayTempo();
		break;
	}
}

void SongScreen::displayTempo()
{
	findField("tempo").lock()->setText(Util::tempoString(sequencer.lock()->getTempo()));
}

void SongScreen::displayLoop()
{
	findField("loop").lock()->setText(loop ? "YES" : "NO");
}

void SongScreen::displaySteps()
{
	auto song = sequencer.lock()->getSong(activeSongIndex).lock();
	int steps = song->getStepCount();
	auto s = sequencer.lock();
	
	auto stepArray = vector<weak_ptr<mpc::lcdgui::Field>>{ findField("step0"), findField("step1"), findField("step2") };
	auto sequenceArray = vector<weak_ptr<mpc::lcdgui::Field>>{ findField("sequence0"), findField("sequence1"), findField("sequence2") };
	auto repsArray = vector<weak_ptr<mpc::lcdgui::Field>>{ findField("reps0"), findField("reps1"), findField("reps2") };

	for (int i = 0; i < 3; i++)
	{
		int stepIndex = i + offset;
	
		if (stepIndex >= 0 && stepIndex < steps)
		{
			stepArray[i].lock()->setTextPadded(stepIndex + 1, " ");
			auto seqname = s->getSequence(song->getStep(stepIndex).lock()->getSequence()).lock()->getName();
			sequenceArray[i].lock()->setText(StrUtil::padLeft(to_string(song->getStep(stepIndex).lock()->getSequence() + 1), "0", 2) + "-" + seqname);
			repsArray[i].lock()->setText(to_string(song->getStep(stepIndex).lock()->getRepeats()));
		}
		else
		{
			stepArray[i].lock()->setText("");
			sequenceArray[i].lock()->setText(stepIndex == steps ? "   (end of song)" : "");
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
	findField("now0").lock()->setTextPadded(sequencer.lock()->getCurrentBarIndex() + 1, "0");
}

void SongScreen::displayNow1()
{
	findField("now1").lock()->setTextPadded(sequencer.lock()->getCurrentBeatIndex() + 1, "0");
}

void SongScreen::displayNow2()
{
	findField("now2").lock()->setTextPadded(sequencer.lock()->getCurrentClockNumber(), "0");
}

void SongScreen::displaySongName()
{
	auto song = sequencer.lock()->getSong(activeSongIndex).lock();
	findField("song").lock()->setText(StrUtil::padLeft(to_string(activeSongIndex + 1), "0", 2) + "-" + song->getName());
}

void SongScreen::setOffset(int i)
{
	if (i < -1)
		return;

	offset = i;

	displaySteps();
	displayTempo();
}

void SongScreen::setSelectedSongIndex(int i)
{
	if (i < 0 || i > 19)
		return;
	
	activeSongIndex = i;

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

	if (msg.compare("bar") == 0)
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
	else if (msg.compare("tempo") == 0)
	{
		displayTempo();
	}
	else if (msg.compare("play") == 0)
	{
		findField("sequence1").lock()->setBlinking(true);
		findField("reps1").lock()->setBlinking(true);
	}
	else if (msg.compare("stop") == 0)
	{
		findField("sequence1").lock()->setBlinking(false);
		findField("reps1").lock()->setBlinking(false);
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

int SongScreen::getActiveSongIndex()
{
	return activeSongIndex;
}

bool SongScreen::isLoopEnabled()
{
	return loop;
}
