#include "NextSeqScreen.hpp"

#include <lcdgui/screens/window/TimingCorrectScreen.hpp>
#include <lcdgui/screens/SequencerScreen.hpp>

#include <sequencer/TempoChangeEvent.hpp>

#include <Util.hpp>

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;

NextSeqScreen::NextSeqScreen(mpc::Mpc& mpc, const int layerIndex)
	: ScreenComponent(mpc, "next-seq", layerIndex)
{
}

void NextSeqScreen::open()
{
	selectNextSqFromScratch = true;

	findLabel("tempo")->setSize(12, 9);
	findField("tempo")->setLocation(18, 11);
	findField("tempo")->setLeftMargin(1);

	displaySq();
	displayNow0();
	displayNow1();
	displayNow2();
	displayTempo();
	displayTempoSource();
	displayTiming();
	displayNextSq();
	
	sequencer.lock()->addObserver(this);

	if (sequencer.lock()->getNextSq() == -1)
		ls->setFocus("sq");
	else
		ls->setFocus("nextsq");
}

void NextSeqScreen::close()
{
	sequencer.lock()->deleteObserver(this);
}

void NextSeqScreen::turnWheel(int i)
{
	init();
	
	if (param == "sq")
	{
		if (sequencer.lock()->isPlaying())
		{
			sequencer.lock()->setNextSq(sequencer.lock()->getCurrentlyPlayingSequenceIndex() + i);
			ls->setFocus("nextsq");
		}
		else
			sequencer.lock()->setActiveSequenceIndex(sequencer.lock()->getActiveSequenceIndex() + i);
	}
	else if (param == "nextsq")
	{
		auto nextSq = sequencer.lock()->getNextSq();
		
		if (nextSq == -1 && i < 0)
        {
            return;
        }

		if (nextSq == -1 && selectNextSqFromScratch)
		{
			nextSq = sequencer.lock()->getActiveSequenceIndex();
			selectNextSqFromScratch = false;
		}
		else
        {
            nextSq += i;
        }

		sequencer.lock()->setNextSq(nextSq);

		displayNextSq();
	}
	else if (param == "timing")
	{
		auto screen = mpc.screens->get<TimingCorrectScreen>();
		auto noteValue = screen->getNoteValue();
		screen->setNoteValue(noteValue + i);
		setLastFocus("timing-correct", "notevalue");
		displayTiming();
	}
	else if (param == "tempo")
	{
		double oldTempo = sequencer.lock()->getTempo();
		double newTempo = oldTempo + (i * 0.1);
		sequencer.lock()->setTempo(newTempo);
		displayTempo();
	}
}

void NextSeqScreen::function(int i)
{
	init();

    if (i == 3 || i == 4)
    {
        auto nextSq = sequencer.lock()->getNextSq();
        sequencer.lock()->setNextSq(-1);
        selectNextSqFromScratch = true;
        displayNextSq();

        if (i == 3 && nextSq != -1)
        {
            sequencer.lock()->stop();
            sequencer.lock()->move(0);
            sequencer.lock()->setActiveSequenceIndex(nextSq);
            sequencer.lock()->playFromStart();
        }
    }
	else if (i == 5)
	{
        mpc.getLayeredScreen()->openScreen<NextSeqPadScreen>();
	}
}

void NextSeqScreen::displaySq()
{
	std::string result = "";

	if (sequencer.lock()->isPlaying())
	{
		result.append(StrUtil::padLeft(std::to_string(sequencer.lock()->getCurrentlyPlayingSequenceIndex() + 1), "0", 2));
		result.append("-");
		result.append(sequencer.lock()->getCurrentlyPlayingSequence()->getName());
		findField("sq")->setText(result);
	}
	else
	{
		result.append(StrUtil::padLeft(std::to_string(sequencer.lock()->getActiveSequenceIndex() + 1), "0", 2));
		result.append("-");
		result.append(sequencer.lock()->getActiveSequence()->getName());
		findField("sq")->setText(result);
	}
}

void NextSeqScreen::displayNextSq()
{
	auto nextSq = sequencer.lock()->getNextSq();
    std::string res = "";

	if (nextSq != -1)
	{
		auto seqName = sequencer.lock()->getSequence(nextSq)->getName();
		res = StrUtil::padLeft(std::to_string(sequencer.lock()->getNextSq() + 1), "0", 2) + "-" + seqName;
	}
	
	findField("nextsq")->setText(res);
}

void NextSeqScreen::displayNow0()
{
	findField("now0")->setTextPadded(sequencer.lock()->getCurrentBarIndex() + 1, "0");
}

void NextSeqScreen::displayNow1()
{
	findField("now1")->setTextPadded(sequencer.lock()->getCurrentBeatIndex() + 1, "0");
}

void NextSeqScreen::displayNow2()
{
	findField("now2")->setTextPadded(sequencer.lock()->getCurrentClockNumber(), "0");
}

void NextSeqScreen::displayTempo()
{
	displayTempoLabel();
	findField("tempo")->setText(Util::tempoString(sequencer.lock()->getTempo()));
}

void NextSeqScreen::displayTempoLabel()
{
	auto currentRatio = -1;
	auto sequence = sequencer.lock()->isPlaying() ? sequencer.lock()->getCurrentlyPlayingSequence() : sequencer.lock()->getActiveSequence();
	for (auto& e : sequence->getTempoChangeEvents())
	{
		if (e->getTick() > sequencer.lock()->getTickPosition())
			break;

		currentRatio = e->getRatio();
	}

	if (currentRatio != 1000)
		findLabel("tempo")->setText(u8"c\u00C0:");
	else
		findLabel("tempo")->setText(u8" \u00C0:");
}

void NextSeqScreen::displayTempoSource()
{
	findField("tempo-source")->setText(sequencer.lock()->isTempoSourceSequenceEnabled() ? "(SEQ)" : "(MAS)");
}

void NextSeqScreen::displayTiming()
{
	auto timingCorrectScreen = mpc.screens->get<TimingCorrectScreen>();
	auto noteValue = timingCorrectScreen->getNoteValue();
	findField("timing")->setText(SequencerScreen::timingCorrectNames[noteValue]);
}

void NextSeqScreen::update(Observable* o, Message message)
{
    const auto msg = std::get<std::string>(message);

	if (msg == "seqnumbername")
	{
		displaySq();
	}
	else if (msg == "bar")
	{
		displayNow0();
	}
	else if (msg == "beat")
	{
		displayNow1();
	}
	else if (msg == "clock")
	{
		displayNow2();
	}
	else if (msg == "now")
	{
		displayNow0();
		displayNow1();
		displayNow2();
	}
	else if (msg == "nextsqvalue")
	{
		displayNextSq();
	}
	else if (msg == "nextsq")
	{
		displayNextSq();
	}
	else if (msg == "nextsqoff")
	{
		selectNextSqFromScratch = true;
		displayNextSq();
	}
	else if (msg == "timing")
	{
		displayTiming();
	}
	else if (msg == "tempo")
	{
		displayTempo();
	}
}
