#include "NextSeqPadScreen.hpp"
#include "controls/Controls.hpp"

using namespace mpc::lcdgui::screens;

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

	sequencer.lock()->addObserver(this);
	mpc.addObserver(this);
}

void NextSeqPadScreen::close()
{
	sequencer.lock()->deleteObserver(this);
	mpc.deleteObserver(this);
}

void NextSeqPadScreen::right()
{
	// Block ScreenComponent::right() default action. Nothing to do.
}

void NextSeqPadScreen::pad(int padIndexWithBank, int velo)
{
	init();

    if (sequencer.lock()->isPlaying() && mpc.getControls()->isF4Pressed())
    {
        if (!sequencer.lock()->getSequence(padIndexWithBank)->isUsed())
        {
            return;
        }

        sequencer.lock()->stop();
        sequencer.lock()->move(0);
        sequencer.lock()->setActiveSequenceIndex(padIndexWithBank);
        sequencer.lock()->playFromStart();
        refreshSeqs();
        return;
    }

	sequencer.lock()->setNextSqPad(padIndexWithBank);
	refreshSeqs();
}

void NextSeqPadScreen::function(int i)
{
	init();
    ScreenComponent::function(i);
	switch (i)
	{
	case 4:
		sequencer.lock()->setNextSq(-1);
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
	auto nextSq = sequencer.lock()->getNextSq();

	if (nextSq == -1)
	{
		findLabel("nextsq")->setText(" ");
		return;
	}

	auto number = StrUtil::padLeft(std::to_string(nextSq + 1), "0", 2);
	auto name = sequencer.lock()->getSequence(nextSq)->getName();
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
	findField("sq")->setText(StrUtil::padLeft(std::to_string(sequencer.lock()->getActiveSequenceIndex() + 1), "0", 2) + "-" + sequencer.lock()->getActiveSequence()->getName());
}

void NextSeqPadScreen::displaySeq(int i)
{
	findField(std::to_string(i + 1))->setText(sequencer.lock()->getSequence(i + bankOffset())->getName().substr(0, 8));
}

void NextSeqPadScreen::setSeqColor(int i)
{
	findField(std::to_string(i + 1))->setInverted(i + bankOffset() == sequencer.lock()->getNextSq());
}

void NextSeqPadScreen::displayNow0()
{
	findField("now0")->setTextPadded(sequencer.lock()->getCurrentBarIndex() + 1, "0");
}

void NextSeqPadScreen::displayNow1()
{
	findField("now1")->setTextPadded(sequencer.lock()->getCurrentBeatIndex() + 1, "0");
}

void NextSeqPadScreen::displayNow2()
{
	findField("now2")->setTextPadded(sequencer.lock()->getCurrentClockNumber(), "0");
}

void NextSeqPadScreen::refreshSeqs()
{
	for (int i = 0; i < 16; i++)
	{
		displaySeq(i);
		setSeqColor(i);
	}
}

void NextSeqPadScreen::update(Observable* observable, Message message)
{
	const auto msg = std::get<std::string>(message);
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

