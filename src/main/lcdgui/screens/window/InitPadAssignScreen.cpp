#include "InitPadAssignScreen.hpp"

#include <controls/BaseSamplerControls.hpp>

using namespace mpc::lcdgui::screens::window;
using namespace mpc::controls;
using namespace std;

InitPadAssignScreen::InitPadAssignScreen(mpc::Mpc& mpc, const int layerIndex) 
	: ScreenComponent(mpc, "init-pad-assign", layerIndex)
{
	baseControls = make_shared<BaseSamplerControls>(mpc);
}

void InitPadAssignScreen::open()
{
	displayInitPadAssign();
}

void InitPadAssignScreen::turnWheel(int i)
{
    init();
	if (param.compare("init-pad-assign") == 0)
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
	findField("init-pad-assign").lock()->setText(initPadAssignIsMaster ? "MASTER" : "PROGRAM");
}
