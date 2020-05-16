#include <lcdgui/screens/window/TimeDisplayScreen.hpp>

#include <ui/sequencer/window/SequencerWindowGui.hpp>

using namespace mpc::lcdgui::screens::window;
using namespace std;

TimeDisplayScreen::TimeDisplayScreen(const int& layer)
	: ScreenComponent("timedisplay", layer)
{
}

void TimeDisplayScreen::turnWheel(int i)
{
	init();
	auto swGui = mpc.getUis().lock()->getSequencerWindowGui();

	if (param.compare("displaystyle") == 0)
	{
		swGui->setDisplayStyle(swGui->getDisplayStyle() + i);
	}
	else if (param.compare("starttime") == 0)
	{
		swGui->setStartTime(swGui->getStartTime() + i);
	}
	else if (param.compare("h") == 0)
	{
		swGui->setH(swGui->getH() + i);
	}
	else if (param.compare("m") == 0)
	{
		swGui->setM(swGui->getM() + i);
	}
	else if (param.compare("s") == 0)
	{
		swGui->setS(swGui->getS() + i);
	}
	else if (param.compare("f") == 0)
	{
		swGui->setF(swGui->getF() + i);
	}
	else if (param.compare("framerate") == 0)
	{
		swGui->setFrameRate(swGui->getFrameRate() + i);
	}
}
