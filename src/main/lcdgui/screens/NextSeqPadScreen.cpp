#include "NextSeqPadScreen.hpp"

#include <sequencer/Sequencer.hpp>

#include <ui/sampler/SamplerGui.hpp>

using namespace mpc::lcdgui::screens;
using namespace moduru::lang;
using namespace std;

NextSeqPadScreen::NextSeqPadScreen(const int& layer)
	: ScreenComponent("nextseqpad", layer)
{
}

void NextSeqPadScreen::open()
{
	for (int i = 0; i < 16; i++) {
		displaySeq(i);
		setOpaque(i);
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
	samplerGui->addObserver(this);
}

void NextSeqPadScreen::close()
{
	sequencer.lock()->deleteObserver(this);
	samplerGui->deleteObserver(this);
}

void NextSeqPadScreen::right()
{
}

void NextSeqPadScreen::pad(int i, int velo, bool repeat, int tick)
{
	init();
	sequencer.lock()->setNextSqPad(i + (mpc.getBank() * 16));
}

void NextSeqPadScreen::turnWheel(int i)
{
	init();
	
	if (param.compare("sq") == 0)
	{
		sequencer.lock()->setActiveSequenceIndex(sequencer.lock()->getActiveSequenceIndex() + i);
	}
}

void NextSeqPadScreen::function(int i)
{
	init();
	switch (i)
	{
	case 5:
		ls.lock()->openScreen("nextseq");
		break;
	}
}

void NextSeqPadScreen::displayNextSq()
{
	auto number = string(sequencer.lock()->getNextSq() == -1 ? "" : StrUtil::padLeft(to_string(sequencer.lock()->getNextSq() + 1), "0", 2));
	auto name = sequencer.lock()->getNextSq() == -1 ? "" : sequencer.lock()->getSequence(sequencer.lock()->getNextSq()).lock()->getName();
	findLabel("nextsq").lock()->setText(number + "-" + name);
}

int NextSeqPadScreen::bankOffset()
{
	return mpc.getBank() * 16;
}

void NextSeqPadScreen::displayBank()
{
	auto letters = vector<string>{ "A", "B", "C", "D" };
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

void NextSeqPadScreen::setOpaque(int i)
{
	findLabel(to_string(i + 1)).lock()->setOpaque(true);
}

void NextSeqPadScreen::displaySeq(int i)
{
	auto lSequencer = sequencer.lock();
	findLabel(to_string(i + 1)).lock()->setText(lSequencer->getSequence(i + bankOffset()).lock()->getName().substr(0, 8));
}

void NextSeqPadScreen::setSeqColor(int i)
{
	auto lSequencer = sequencer.lock();

	auto label = findLabel(to_string(i + 1)).lock();
	if (i + bankOffset() == lSequencer->getNextSq())
	{
		//label->setForeground(false);
		label->setInverted(true);
	}
	else {
		label->setInverted(false);
	}
}

void NextSeqPadScreen::displayNow0()
{
	auto lSequencer = sequencer.lock();
	findField("now0").lock()->setTextPadded(lSequencer->getCurrentBarNumber() + 1, "0");
}

void NextSeqPadScreen::displayNow1()
{
	auto lSequencer = sequencer.lock();
	findField("now1").lock()->setTextPadded(lSequencer->getCurrentBeatNumber() + 1, "0");
}

void NextSeqPadScreen::displayNow2()
{
	auto lSequencer = sequencer.lock();
	findField("now2").lock()->setTextPadded(lSequencer->getCurrentClockNumber(), "0");
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
	
	if (msg.compare("soloenabled") == 0 || msg.compare("bank") == 0) {
		displayBank();
		displaySeqNumbers();
		refreshSeqs();
	}
	else if (msg.compare("seqnumbername") == 0) {
		displaySq();
		refreshSeqs();
	}
	else if (msg.compare("nextsqoff") == 0) {
		refreshSeqs();
		displayNextSq();
	}
	else if (msg.compare("nextsqvalue") == 0 || msg.compare("nextsq") == 0) {
		refreshSeqs();
		displayNextSq();
	}
	else if (msg.compare("now") == 0 || msg.compare("clock") == 0) {
		displayNow0();
		displayNow1();
		displayNow2();
	}
}
