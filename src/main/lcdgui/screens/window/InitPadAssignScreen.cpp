#include "InitPadAssignScreen.hpp"

#include <ui/sampler/SamplerGui.hpp>
#include <ui/sampler/window/SamplerWindowGui.hpp>
#include <sampler/Program.hpp>
#include <sampler/Sampler.hpp>

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
    setInitPadAssign(i > 0);
}


void InitPadAssignScreen::setInitPadAssign(bool b)
{
	initPadAssignIsMaster = b;
	displayInitPadAssign();
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
		
		samplerGui->setPadAssignMaster(initPadAssignIsMaster);
		ls.lock()->openScreen("programassign");
		break;
	}
}
