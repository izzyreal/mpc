#include <lcdgui/screens/window/LoopBarsScreen.hpp>

#include <sequencer/Sequence.hpp>

using namespace mpc::lcdgui::screens::window;
using namespace std;

LoopBarsScreen::LoopBarsScreen(const int& layer)
	: ScreenComponent("loopbars", layer)
{
}

void LoopBarsScreen::turnWheel(int i)
{
	init();
	auto seq = sequencer.lock()->getActiveSequence().lock();

	if (param.compare("firstbar") == 0)
	{
		seq->setFirstLoopBar(seq->getFirstLoopBar() + i);
	}
	else if (param.compare("lastbar") == 0)
	{
		seq->setLastLoopBar(seq->getLastLoopBar() + i);
	}
	else if (param.compare("numberofbars") == 0 && i < 0)
	{
		if (seq->isLastLoopBarEnd())
		{
			seq->setLastLoopBar(seq->getLastLoopBar() - 2);
		}
		else if (seq->getLastLoopBar() > seq->getFirstLoopBar())
		{
			seq->setLastLoopBar(seq->getLastLoopBar() - 1);
		}
	}
	else if (param.compare("numberofbars") == 0) {
		seq->setLastLoopBar(seq->getLastLoopBar() + i);
	}
}
