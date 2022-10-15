#include "InitPadAssignScreen.hpp"

using namespace mpc::lcdgui::screens::window;
using namespace mpc::controls;
using namespace std;

InitPadAssignScreen::InitPadAssignScreen(mpc::Mpc& mpc, const int layerIndex) 
	: ScreenComponent(mpc, "init-pad-assign", layerIndex)
{
}

void InitPadAssignScreen::open()
{
	findField("init-pad-assign").lock()->setAlignment(Alignment::Centered);
	displayInitPadAssign();
}

void InitPadAssignScreen::turnWheel(int i)
{
    init();
	if (param == "init-pad-assign")
	{
		initPadAssignIsMaster = i > 0;
		displayInitPadAssign();
	}
}

void InitPadAssignScreen::function(int i)
{
	init();
	
	ScreenComponent::function(i);

	switch (i)
	{
	case 4:
		if (initPadAssignIsMaster)
		{
			sampler->setMasterPadAssign(*sampler->getInitMasterPadAssign());
		}
		else {
			program.lock()->initPadAssign();
		}
		
		openScreen("program-assign");
		break;
	}
}

void InitPadAssignScreen::displayInitPadAssign()
{
	findField("init-pad-assign").lock()->setText(initPadAssignIsMaster ? "MASTER" : "PROGRAM");
}
