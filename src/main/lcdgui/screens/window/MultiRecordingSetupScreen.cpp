#include "MultiRecordingSetupScreen.hpp"

#include <lcdgui/Field.hpp>
#include <ui/sequencer/window/MultiRecordingSetupLine.hpp>
#include <ui/sequencer/window/SequencerWindowGui.hpp>
#include <sequencer/Sequence.hpp>
#include <sequencer/Track.hpp>

#include <lang/StrUtil.hpp>

using namespace mpc::lcdgui::screens::window;
using namespace std;

MultiRecordingSetupScreen::MultiRecordingSetupScreen(const int& layer)
	: ScreenComponent("multirecordingsetup", layer)
{
}

void MultiRecordingSetupScreen::init()
{
	BaseControls::init();
	
	yPos = 0;
	
	if (param.length() == 2)
	{
		yPos = stoi(param.substr(1, 2));
	}
}

void MultiRecordingSetupScreen::left()
{
	init();
	if (csn.compare("multirecordingsetup") == 0 && param[0] == 'a')
	{
		return;
	}
	BaseControls::left();
}

void MultiRecordingSetupScreen::right()
{
	init();
	if (param[0] == 'c') return;
	BaseControls::right();
}

void MultiRecordingSetupScreen::turnWheel(int i)
{
	init();
	
	auto seq = sequencer.lock()->getActiveSequence().lock();
	auto swGui = mpc.getUis().lock()->getSequencerWindowGui();

	if (param[0] == 'a')
	{
		if (i > 0)
		{
			if (yPos == 0)
			{
				string res = param.substr(0, 1) + to_string(yPos + 1);
				ls.lock()->setFocus(param.substr(0, 1).append(to_string(yPos + 1)));
			}
			else if (yPos == 1)
			{
				ls.lock()->setFocus(param.substr(0, 1).append(to_string(yPos + 1)));
			}
			else if (yPos == 2)
			{
				swGui->setMrsYOffset(swGui->getMrsYOffset() + 1);
			}
		}
		else if (i < 0)
		{
			if (yPos == 0)
			{
				swGui->setMrsYOffset(swGui->getMrsYOffset() - 1);
			}
			else if (yPos == 1)
			{
				ls.lock()->setFocus(param.substr(0, 1).append(to_string(yPos - 1)));
			}
			else if (yPos == 2)
			{
				ls.lock()->setFocus(param.substr(0, 1).append(to_string(yPos - 1)));
			}
		}
	}
	else if (param[0] == 'b')
	{
		swGui->setMrsTrack(yPos + swGui->getMrsYOffset(), (*swGui->getVisibleMrsLines())[yPos]->getTrack() + i);
		swGui->setMrsOut(yPos + swGui->getMrsYOffset(), seq->getTrack((*swGui->getVisibleMrsLines())[yPos]->getTrack()).lock()->getDevice());
	}
	else if (param[0] == 'c')
	{
		swGui->setMrsOut(yPos + swGui->getMrsYOffset(), (*swGui->getVisibleMrsLines())[yPos]->getOut() + i);
		seq->getTrack((*swGui->getVisibleMrsLines())[yPos]->getTrack()).lock()->setDeviceNumber((*swGui->getVisibleMrsLines())[yPos]->getOut());
	}
}

void MultiRecordingSetupScreen::up()
{
	init();
	auto swGui = mpc.getUis().lock()->getSequencerWindowGui();

	if (yPos == 0)
	{
		swGui->setMrsYOffset(swGui->getMrsYOffset() - 1);
	}
	else if (yPos == 1)
	{
		ls.lock()->setFocus(param.substr(0, 1).append(to_string(yPos - 1)));
	}
	else if (yPos == 2)
	{
		ls.lock()->setFocus(param.substr(0, 1).append(to_string(yPos - 1)));
	}
}

void MultiRecordingSetupScreen::down()
{
	init();
	auto swGui = mpc.getUis().lock()->getSequencerWindowGui(); 
	
	if (yPos == 0)
	{
		string res = param.substr(0, 1) + to_string(yPos + 1);
		ls.lock()->setFocus(param.substr(0, 1).append(to_string(yPos + 1)));
	}
	else if (yPos == 1)
	{
		ls.lock()->setFocus(param.substr(0, 1).append(to_string(yPos + 1)));
	}
	else if (yPos == 2)
	{
		swGui->setMrsYOffset(swGui->getMrsYOffset() + 1);
	}
}
