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

SongScreen::~SongScreen()
{
	if (blinkThread.joinable())
		blinkThread.join();
}

void SongScreen::open()
{
	findField("loop").lock()->setAlignment(Alignment::Centered);
	for (int i = 0; i < 3; i++)
	{
		findField("step" + to_string(i)).lock()->setAlignment(Alignment::Centered);
		findField("reps" + to_string(i)).lock()->setAlignment(Alignment::Centered);
	}

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
		if (offset == -1 || sequencer.lock()->isPlaying())
			return;
	
		setOffset(offset - 1);
		sequencer.lock()->setActiveSequenceIndex(sequencer.lock()->getSongSequenceIndex());
		sequencer.lock()->setBar(0);
	}
	else
	{
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
	if (sequencer.lock()->isPlaying())
		return;

	init();

	auto song = sequencer.lock()->getSong(activeSongIndex).lock();

	if (!song->isUsed())
		song->setUsed(true);

	if (param.compare("loop") == 0)
		openScreen("loop-song");
	else if (param.compare("song") == 0)
		openScreen("song-window");
	else if (param.compare("tempo") == 0 || param.compare("tempo-source") == 0)
		openScreen("ignore-tempo-change");
}

void SongScreen::down()
{
	init();

	if (param.compare("step1") == 0 || param.compare("sequence1") == 0 || param.compare("reps1") == 0)
	{	
		auto song = sequencer.lock()->getSong(activeSongIndex).lock();
		
		if (offset == song->getStepCount() - 1 || sequencer.lock()->isPlaying())
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
		auto step = song->getStep(offset + 1).lock();
		
		step->setSequence(step->getSequence() + i);		
		sequencer.lock()->setActiveSequenceIndex(step->getSequence());
		sequencer.lock()->setBar(0);
		displayNow0();
		displayNow1();
		displayNow2();
		displaySteps();
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
		setActiveSongIndex(activeSongIndex + i);
		setOffset(-1);
		init();

		if (song->isUsed() && song->getStepCount() != 0)
			sequencer.lock()->setActiveSequenceIndex(song->getStep(0).lock()->getSequence());
	}
	else if (param.compare("tempo") == 0 && !sequencer.lock()->isTempoSourceSequenceEnabled())
	{
		sequencer.lock()->setTempo(sequencer.lock()->getTempo() + (i * 0.1));
	}
	else if (param.compare("tempo-source") == 0)
	{
		sequencer.lock()->setTempoSourceSequence(i > 0);
		displayTempoSource();
		displayTempo();
	}
	else if (param.compare("loop") == 0)
	{
		setLoop(i > 0);
	}
	else if (param.compare("step1") == 0)
	{
		setOffset(offset + i);
	}
}

void SongScreen::function(int i)
{
	if (sequencer.lock()->isPlaying())
		return;

	init();
	auto song = sequencer.lock()->getSong(activeSongIndex).lock();
	
	switch (i)
	{
	case 4:
		if (sequencer.lock()->isPlaying())
			return;

		song->deleteStep(offset + 1);
		displaySteps();
		displayNow0();
		displayNow1();
		displayNow2();
		displayTempo();
		break;
	case 5:
		if (param.compare("step1") != 0 && param.compare("sequence1") != 0)
			return;

		song->insertStep(offset + 1);
		
		auto candidate = offset + 1;

		if (candidate + 1 >= song->getStepCount())
			candidate -= 1;

		setOffset(candidate);
	
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
			stepArray[i].lock()->setText(to_string(stepIndex + 1));
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
	findField("tempo-source").lock()->setText(sequencer.lock()->isTempoSourceSequenceEnabled() ? "SEQ" : "MAS");
}

void SongScreen::displayNow0()
{
	int pastBars = 0;

	auto song = sequencer.lock()->getSong(activeSongIndex).lock();

	for (int i = 0; i < offset + 1; i++)
	{
		if (i >= song->getStepCount())
			break;

		auto step = song->getStep(i).lock();
		auto seq = sequencer.lock()->getSequence(step->getSequence()).lock();

		if (!seq->isUsed())
			continue;

		auto bars = (seq->getLastBarIndex() + 1) * step->getRepeats();

		pastBars += bars;
	}

	pastBars += sequencer.lock()->getPlayedStepRepetitions() * (sequencer.lock()->getActiveSequence().lock()->getLastBarIndex() + 1);
	
	findField("now0").lock()->setTextPadded(sequencer.lock()->getCurrentBarIndex() + 1 + pastBars, "0");
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
		i = -1;

	auto song = sequencer.lock()->getSong(activeSongIndex).lock();

	if (i >= song->getStepCount() - 1)
		i = song->getStepCount() - 1;

	offset = i;

	displaySteps();
	displayTempo();
}

void SongScreen::setActiveSongIndex(int i)
{
	if (i < 0 || i > 19)
		return;
	
	activeSongIndex = i;

	displaySongName();
	displaySteps();
	displayTempoSource();
	displayTempo();
	displayNow0();
	displayNow1();
	displayNow2();
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
		auto blinkThreadFunction = [&]
		{
			const auto blinkInterval = 380;
			const auto checkInterval = 38;
			bool on = true;
			
			while (sequencer.lock()->isPlaying())
			{
				int counter = 0;

				while (counter++ * checkInterval < blinkInterval && sequencer.lock()->isPlaying())
					this_thread::sleep_for(chrono::milliseconds(checkInterval));

				auto playing = sequencer.lock()->isPlaying();

				findField("sequence1").lock()->setRectangleOnly(on && playing);
				findField("reps1").lock()->setRectangleOnly(on && playing);
				on = !on;
			}
		};

		if (blinkThread.joinable())
			blinkThread.join();

		blinkThread = std::thread(blinkThreadFunction);
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
