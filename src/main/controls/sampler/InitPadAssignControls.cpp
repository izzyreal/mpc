#include "InitPadAssignControls.hpp"

#include <ui/sampler/SamplerGui.hpp>
#include <ui/sampler/window/SamplerWindowGui.hpp>
#include <sampler/Program.hpp>
#include <sampler/Sampler.hpp>

using namespace mpc::controls::sampler;
using namespace std;

InitPadAssignControls::InitPadAssignControls() 
	: AbstractSamplerControls()
{
}

void InitPadAssignControls::turnWheel(int i)
{
    init();
    swGui->setInitPadAssignMaster(i > 0);
}

void InitPadAssignControls::function(int i)
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
