#include "LoopBarsScreen.hpp"

#include <sequencer/Sequence.hpp>

using namespace mpc::lcdgui::screens::window;
using namespace std;

LoopBarsScreen::LoopBarsScreen(mpc::Mpc& mpc, const int layerIndex)
	: ScreenComponent(mpc, "loop-bars-window", layerIndex)
{
}

void LoopBarsScreen::open()
{
	displayFirstBar();
	displayLastBar();
	displayNumberOfBars();
}

void LoopBarsScreen::turnWheel(int i)
{
	init();
	auto seq = sequencer.lock()->getActiveSequence().lock();

	if (param.compare("firstbar") == 0)
	{
		seq->setFirstLoopBarIndex(seq->getFirstLoopBarIndex() + i);
		displayFirstBar();
		displayLastBar();
		displayNumberOfBars();
	}
	else if (param.compare("lastbar") == 0)
	{
		seq->setLastLoopBarIndex(seq->getLastLoopBarIndex() + i);
		displayLastBar();
		displayFirstBar();
		displayNumberOfBars();
	}
	else if (param.compare("numberofbars") == 0 && i < 0)
	{
		if (seq->isLastLoopBarEnd())
		{
			seq->setLastLoopBarIndex(seq->getLastLoopBarIndex() - 2);
			displayLastBar();
			displayNumberOfBars();
		}
		else if (seq->getLastLoopBarIndex() >= seq->getFirstLoopBarIndex())
		{
			seq->setLastLoopBarIndex(seq->getLastLoopBarIndex() - 1);
			displayLastBar();
			displayNumberOfBars();
		}
	}
	else if (param.compare("numberofbars") == 0)
	{
		seq->setLastLoopBarIndex(seq->getLastLoopBarIndex() + i);
		displayLastBar();
		displayNumberOfBars();
	}
}

void LoopBarsScreen::displayLastBar()
{
	auto seq = sequencer.lock()->getActiveSequence().lock();
	
	if (seq->isLastLoopBarEnd())
	{
		findField("lastbar").lock()->setText("END");
	}
	else {
		findField("lastbar").lock()->setText(to_string(seq->getLastLoopBarIndex() + 1));
	}
}

void LoopBarsScreen::displayNumberOfBars()
{
	auto seq = sequencer.lock()->getActiveSequence().lock();
	findField("numberofbars").lock()->setText(to_string(seq->getLastLoopBarIndex() - seq->getFirstLoopBarIndex() + 1));
}

void LoopBarsScreen::displayFirstBar()
{
	auto seq = sequencer.lock()->getActiveSequence().lock();
	findField("firstbar").lock()->setText(to_string(seq->getFirstLoopBarIndex() + 1));
}
