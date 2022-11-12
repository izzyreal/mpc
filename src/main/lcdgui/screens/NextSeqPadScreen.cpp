#include "NextSeqPadScreen.hpp"

using namespace mpc::lcdgui::screens;
using namespace moduru::lang;

NextSeqPadScreen::NextSeqPadScreen(mpc::Mpc& mpc, const int layerIndex)
	: ScreenComponent(mpc, "next-seq-pad", layerIndex)
{
}

void NextSeqPadScreen::open()
{
	for (int i = 0; i < 16; i++)
	{
		findField(std::to_string(i + 1))->setFocusable(false);
		displaySeq(i);
		setSeqColor(i);
	}
	
	displaySq();
	displayNow0();
	displayNow1();
	displayNow2();
	displayBank();
	displaySeqNumbers();
	displayNextSq();

	sequencer->addObserver(this);
	mpc.addObserver(this);
}

void NextSeqPadScreen::close()
{
	sequencer->deleteObserver(this);
	mpc.deleteObserver(this);
}

void NextSeqPadScreen::right()
{
	// Block ScreenComponent::right() default action. Nothing to do.
}

void NextSeqPadScreen::pad(int padIndexWithBank, int velo)
{
	init();

    if (sequencer->isPlaying() && mpc.getControls().lock()->isF4Pressed())
    {
        if (!sequencer->getSequence(padIndexWithBank)->isUsed())
        {
            return;
        }

        sequencer->stop();
        sequencer->move(0);
        sequencer->setActiveSequenceIndex(padIndexWithBank);
        sequencer->playFromStart();
        refreshSeqs();
        return;
    }

	sequencer->setNextSqPad(padIndexWithBank);
	refreshSeqs();
}

void NextSeqPadScreen::function(int i)
{
	init();
    ScreenComponent::function(i);
	switch (i)
	{
	case 4:
		sequencer->setNextSq(-1);
		displayNextSq();
		refreshSeqs();
		break;
	case 5:
		openScreen("next-seq");
		break;
	}
}

void NextSeqPadScreen::displayNextSq()
{
	auto nextSq = sequencer->getNextSq();

	if (nextSq == -1)
	{
		findLabel("nextsq")->setText(" ");
		return;
	}

	auto number = StrUtil::padLeft(std::to_string(nextSq + 1), "0", 2);
	auto name = sequencer->getSequence(nextSq)->getName();
	findLabel("nextsq")->setText(number + "-" + name);
}

int NextSeqPadScreen::bankOffset()
{
	return mpc.getBank() * 16;
}

void NextSeqPadScreen::displayBank()
{
	findLabel("bank")->setText(letters[mpc.getBank()]);
}

void NextSeqPadScreen::displaySeqNumbers()
{
	std::vector<std::string> seqn{ "01-16", "17-32", "33-48", "49-64" };
	findLabel("seqnumbers")->setText(seqn[mpc.getBank()]);
}

void NextSeqPadScreen::displaySq()
{
	findField("sq")->setText(StrUtil::padLeft(std::to_string(sequencer->getActiveSequenceIndex() + 1), "0", 2) + "-" + sequencer->getActiveSequence()->getName());
}

void NextSeqPadScreen::displaySeq(int i)
{
	findField(std::to_string(i + 1))->setText(sequencer->getSequence(i + bankOffset())->getName().substr(0, 8));
}

void NextSeqPadScreen::setSeqColor(int i)
{
	findField(std::to_string(i + 1))->setInverted(i + bankOffset() == sequencer->getNextSq());
}

void NextSeqPadScreen::displayNow0()
{
	findField("now0")->setTextPadded(sequencer->getCurrentBarIndex() + 1, "0");
}

void NextSeqPadScreen::displayNow1()
{
	findField("now1")->setTextPadded(sequencer->getCurrentBeatIndex() + 1, "0");
}

void NextSeqPadScreen::displayNow2()
{
	findField("now2")->setTextPadded(sequencer->getCurrentClockNumber(), "0");
}

void NextSeqPadScreen::refreshSeqs()
{
	for (int i = 0; i < 16; i++)
	{
		displaySeq(i);
		setSeqColor(i);
	}
}

void NextSeqPadScreen::update(moduru::observer::Observable* observable, nonstd::any message)
{
	auto msg = nonstd::any_cast<std::string>(message);
	if (msg == "bank")
    {
        displayBank();
        displaySeqNumbers();

        for (int i = 0; i < 16; i++)
        {
            displaySeq(i);
        }
    }
	else if (msg == "soloenabled")
	{
		refreshSeqs();
	}
	else if (msg == "seqnumbername")
	{
		displaySq();
		refreshSeqs();
	}
	else if (msg == "nextsqoff")
	{
		refreshSeqs();
		displayNextSq();
	}
	else if (msg == "nextsqvalue" || msg == "nextsq")
	{
		refreshSeqs();
		displayNextSq();
	}
	else if (msg == "now" || msg == "clock")
	{
		displayNow0();
		displayNow1();
		displayNow2();
	}
}
