#include "NextSeqScreen.hpp"

#include <lcdgui/screens/window/TimingCorrectScreen.hpp>
#include <lcdgui/screens/SequencerScreen.hpp>

#include <sequencer/TempoChangeEvent.hpp>

#include <Util.hpp>

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;
using namespace moduru::lang;
using namespace std;

NextSeqScreen::NextSeqScreen(mpc::Mpc& mpc, const int layerIndex)
	: ScreenComponent(mpc, "next-seq", layerIndex)
{
}

void NextSeqScreen::open()
{
	selectNextSqFromScratch = true;

	findLabel("tempo").lock()->setSize(12, 9);
	findField("tempo").lock()->setLocation(18, 11);
	findField("tempo").lock()->setLeftMargin(1);

	displaySq();
	displayNow0();
	displayNow1();
	displayNow2();
	displayTempo();
	displayTempoSource();
	displayTiming();
	displayNextSq();
	
	sequencer->addObserver(this);

	if (sequencer->getNextSq() == -1)
		ls.lock()->setFocus("sq");
	else
		ls.lock()->setFocus("nextsq");
}

void NextSeqScreen::close()
{
	sequencer->deleteObserver(this);
}

void NextSeqScreen::turnWheel(int i)
{
	init();
	
	if (param.compare("sq") == 0)
	{
		if (sequencer->isPlaying())
		{
			sequencer->setNextSq(sequencer->getCurrentlyPlayingSequenceIndex() + i);
			ls.lock()->setFocus("nextsq");
		}
		else
			sequencer->setActiveSequenceIndex(sequencer->getActiveSequenceIndex() + i);
	}
	else if (param.compare("nextsq") == 0)
	{
		auto nextSq = sequencer->getNextSq();
		
		if (nextSq == -1 && i < 0)
			return;

		if (nextSq == -1 && selectNextSqFromScratch)
		{
			nextSq = sequencer->getActiveSequenceIndex();
			selectNextSqFromScratch = false;
		}
		else
			nextSq += i;

		sequencer->setNextSq(nextSq);

		displayNextSq();
	}
	else if (param.compare("timing") == 0)
	{
		auto screen = mpc.screens->get<TimingCorrectScreen>("timing-correct");
		auto noteValue = screen->getNoteValue();
		screen->setNoteValue(noteValue + i);
		setLastFocus("timing-correct", "notevalue");
		displayTiming();
	}
	else if (param.compare("tempo") == 0)
	{
		double oldTempo = sequencer->getTempo();
		double newTempo = oldTempo + (i * 0.1);
		sequencer->setTempo(newTempo);
		displayTempo();
	}
}

void NextSeqScreen::function(int i)
{
	init();

    if (i == 3 || i == 4)
    {
        auto nextSq = sequencer->getNextSq();
        sequencer->setNextSq(-1);
        selectNextSqFromScratch = true;
        displayNextSq();

        if (i == 3)
        {
            sequencer->stop();
            sequencer->move(0);
            sequencer->setActiveSequenceIndex(nextSq);
            sequencer->playFromStart();
        }
    }
	else if (i == 5)
	{
		openScreen("next-seq-pad");
	}
}

void NextSeqScreen::displaySq()
{
	string result = "";

	if (sequencer->isPlaying())
	{
		result.append(StrUtil::padLeft(to_string(sequencer->getCurrentlyPlayingSequenceIndex() + 1), "0", 2));
		result.append("-");
		result.append(sequencer->getCurrentlyPlayingSequence().lock()->getName());
		findField("sq").lock()->setText(result);
	}
	else
	{
		result.append(StrUtil::padLeft(to_string(sequencer->getActiveSequenceIndex() + 1), "0", 2));
		result.append("-");
		result.append(sequencer->getActiveSequence()->getName());
		findField("sq").lock()->setText(result);
	}
}

void NextSeqScreen::displayNextSq()
{
	auto nextSq = sequencer->getNextSq();
	string res = "";

	if (nextSq != -1)
	{
		auto seqName = sequencer->getSequence(nextSq).lock()->getName();
		res = StrUtil::padLeft(to_string(sequencer->getNextSq() + 1), "0", 2) + "-" + seqName;
	}
	
	findField("nextsq").lock()->setText(res);
}

void NextSeqScreen::displayNow0()
{
	findField("now0").lock()->setTextPadded(sequencer->getCurrentBarIndex() + 1, "0");
}

void NextSeqScreen::displayNow1()
{
	findField("now1").lock()->setTextPadded(sequencer->getCurrentBeatIndex() + 1, "0");
}

void NextSeqScreen::displayNow2()
{
	findField("now2").lock()->setTextPadded(sequencer->getCurrentClockNumber(), "0");
}

void NextSeqScreen::displayTempo()
{
	displayTempoLabel();
	findField("tempo").lock()->setText(Util::tempoString(sequencer->getTempo()));
}

void NextSeqScreen::displayTempoLabel()
{
	auto currentRatio = -1;
	auto sequence = sequencer->isPlaying() ? sequencer->getCurrentlyPlayingSequence().lock() : sequencer->getActiveSequence();
	for (auto& e : sequence->getTempoChangeEvents())
	{
		auto tce = e.lock();

		if (tce->getTick() > sequencer->getTickPosition())
			break;

		currentRatio = tce->getRatio();
	}

	if (currentRatio != 1000)
		findLabel("tempo").lock()->setText(u8"c\u00C0:");
	else
		findLabel("tempo").lock()->setText(u8" \u00C0:");
}

void NextSeqScreen::displayTempoSource()
{
	findField("tempo-source").lock()->setText(sequencer->isTempoSourceSequenceEnabled() ? "(SEQ)" : "(MAS)");
}

void NextSeqScreen::displayTiming()
{
	auto timingCorrectScreen = mpc.screens->get<TimingCorrectScreen>("timing-correct");
	auto noteValue = timingCorrectScreen->getNoteValue();
	findField("timing").lock()->setText(SequencerScreen::timingCorrectNames[noteValue]);
}

void NextSeqScreen::update(moduru::observer::Observable* o, nonstd::any arg)
{
	string s = nonstd::any_cast<string>(arg);

	if (s.compare("seqnumbername") == 0)
	{
		displaySq();
	}
	else if (s.compare("bar") == 0)
	{
		displayNow0();
	}
	else if (s.compare("beat") == 0)
	{
		displayNow1();
	}
	else if (s.compare("clock") == 0)
	{
		displayNow2();
	}
	else if (s.compare("now") == 0)
	{
		displayNow0();
		displayNow1();
		displayNow2();
	}
	else if (s.compare("nextsqvalue") == 0)
	{
		displayNextSq();
	}
	else if (s.compare("nextsq") == 0)
	{
		displayNextSq();
	}
	else if (s.compare("nextsqoff") == 0)
	{
		selectNextSqFromScratch = true;
		displayNextSq();
	}
	else if (s.compare("timing") == 0)
	{
		displayTiming();
	}
	else if (s.compare("tempo") == 0)
	{
		displayTempo();
	}
}
