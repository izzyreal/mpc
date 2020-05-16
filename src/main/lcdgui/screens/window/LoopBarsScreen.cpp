#include <lcdgui/screens/window/LoopBarsScreen.hpp>

#include <sequencer/Sequence.hpp>

using namespace mpc::lcdgui::screens::window;
using namespace std;

LoopBarsScreen::LoopBarsScreen(const int& layer)
	: ScreenComponent("loopbarswindow", layer)
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
		seq->setFirstLoopBar(seq->getFirstLoopBar() + i);
		displayFirstBar();
		displayNumberOfBars();
	}
	else if (param.compare("lastbar") == 0)
	{
		seq->setLastLoopBar(seq->getLastLoopBar() + i);
		displayLastBar();
		displayNumberOfBars();
	}
	else if (param.compare("numberofbars") == 0 && i < 0)
	{
		if (seq->isLastLoopBarEnd())
		{
			seq->setLastLoopBar(seq->getLastLoopBar() - 2);
			displayLastBar();
			displayNumberOfBars();
		}
		else if (seq->getLastLoopBar() > seq->getFirstLoopBar())
		{
			seq->setLastLoopBar(seq->getLastLoopBar() - 1);
			displayLastBar();
			displayNumberOfBars();
		}
	}
	else if (param.compare("numberofbars") == 0) {
		seq->setLastLoopBar(seq->getLastLoopBar() + i);
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
		findField("lastbar").lock()->setText(to_string(seq->getLastLoopBar() + 1));
	}
}

void LoopBarsScreen::displayNumberOfBars()
{
	auto seq = sequencer.lock()->getActiveSequence().lock();
	findField("numberofbars").lock()->setText(to_string(seq->getLastLoopBar() - seq->getFirstLoopBar() + 1));
}

void LoopBarsScreen::displayFirstBar()
{
	auto seq = sequencer.lock()->getActiveSequence().lock();
	findField("firstbar").lock()->setText(to_string(seq->getFirstLoopBar() + 1));
}
