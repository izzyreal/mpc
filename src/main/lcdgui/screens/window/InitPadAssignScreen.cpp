#include "InitPadAssignScreen.hpp"

using namespace mpc::lcdgui::screens::window;
using namespace std;

InitPadAssignScreen::InitPadAssignScreen(const int layerIndex) 
	: ScreenComponent("initpadassign", layerIndex)
{
}

void InitPadAssignScreen::open()
{
	displayInitPadAssign();
}

void InitPadAssignScreen::turnWheel(int i)
{
    init();
	if (param.compare("initpadassign") == 0)
	{
		initPadAssignIsMaster = i > 0;
		displayInitPadAssign();
	}
}

void InitPadAssignScreen::function(int i)
{
	init();
	
	switch (i)
	{
	case 4:
		if (initPadAssignIsMaster)
		{
			sampler.lock()->setMasterPadAssign(*sampler.lock()->getInitMasterPadAssign());
		}
		else {
			program.lock()->initPadAssign();
		}
		
		ls.lock()->openScreen("program-assign");
		break;
	}
}

void InitPadAssignScreen::displayInitPadAssign()
{
	findField("initpadassign").lock()->setText(initPadAssignIsMaster ? "MASTER" : "PROGRAM");
}
