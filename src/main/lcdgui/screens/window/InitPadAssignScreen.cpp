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

void InitPadAssignScreen::turnWheel(int i)
{
    init();
    swGui->setInitPadAssignMaster(i > 0);
}

void InitPadAssignScreen::function(int i)
{
	init();
	
	auto lLs = ls.lock();
	switch (i) {
	case int(4) :
		if (swGui->isInitPadAssignMaster()) {
			sampler.lock()->setMasterPadAssign(*sampler.lock()->getInitMasterPadAssign());
		}
		else {
			program.lock()->initPadAssign();
		}
		samplerGui->setPadAssignMaster(swGui->isInitPadAssignMaster());
		lLs->openScreen("programassign");
		break;
	}
}
