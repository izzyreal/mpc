#include "NextSeqPadScreen.hpp"

using namespace mpc::lcdgui::screens;
using namespace moduru::lang;
using namespace std;

NextSeqPadScreen::NextSeqPadScreen(mpc::Mpc& mpc, const int layerIndex)
	: ScreenComponent(mpc, "next-seq-pad", layerIndex)
{
}

void NextSeqPadScreen::open()
{
	for (int i = 0; i < 16; i++)
	{
		findField(to_string(i + 1)).lock()->setFocusable(false);
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

void NextSeqPadScreen::pad(int padIndexWithBank, int velo, bool repeat, int tick)
{
	init();

    if (sequencer.lock()->isPlaying() && mpc.getControls().lock()->isF4Pressed())
    {
        if (!sequencer.lock()->getSequence(padIndexWithBank).lock()->isUsed())
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
		findLabel("nextsq").lock()->setText(" ");
		return;
	}

	auto number = StrUtil::padLeft(to_string(nextSq + 1), "0", 2);
	auto name = sequencer.lock()->getSequence(nextSq).lock()->getName();
	findLabel("nextsq").lock()->setText(number + "-" + name);
}

int NextSeqPadScreen::bankOffset()
{
	return mpc.getBank() * 16;
}

void NextSeqPadScreen::displayBank()
{
	findLabel("bank").lock()->setText(letters[mpc.getBank()]);
}

void NextSeqPadScreen::displaySeqNumbers()
{
	auto seqn = vector<string>{ "01-16", "17-32", "33-48", "49-64" };
	findLabel("seqnumbers").lock()->setText(seqn[mpc.getBank()]);
}

void NextSeqPadScreen::displaySq()
{
	auto lSequencer = sequencer.lock();
	findField("sq").lock()->setText(StrUtil::padLeft(to_string(lSequencer->getActiveSequenceIndex() + 1), "0", 2) + "-" + lSequencer->getActiveSequence().lock()->getName());
}

void NextSeqPadScreen::displaySeq(int i)
{
	findField(to_string(i + 1)).lock()->setText(sequencer.lock()->getSequence(i + bankOffset()).lock()->getName().substr(0, 8));
}

void NextSeqPadScreen::setSeqColor(int i)
{
	findField(to_string(i + 1)).lock()->setInverted(i + bankOffset() == sequencer.lock()->getNextSq());
}

void NextSeqPadScreen::displayNow0()
{
	findField("now0").lock()->setTextPadded(sequencer.lock()->getCurrentBarIndex() + 1, "0");
}

void NextSeqPadScreen::displayNow1()
{
	findField("now1").lock()->setTextPadded(sequencer.lock()->getCurrentBeatIndex() + 1, "0");
}

void NextSeqPadScreen::displayNow2()
{
	findField("now2").lock()->setTextPadded(sequencer.lock()->getCurrentClockNumber(), "0");
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
	string msg = nonstd::any_cast<string>(message);
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
