#include "CopyProgramControls.hpp"

#include <ui/sampler/window/SamplerWindowGui.hpp>

#include <sampler/Sampler.hpp>

using namespace mpc::controls::sampler::dialog;
using namespace std;

CopyProgramControls::CopyProgramControls() 
	: AbstractSamplerControls()
{
}

void CopyProgramControls::function(int i)
{
	init();
	auto lLs = ls.lock();
	switch (i) {
	case int(3) :
		lLs->openScreen("program");
		break;
	case 4:
		if (swGui->getPgm0() == swGui->getPgm1()) {
			return;
		}
		lLs->openScreen("program");
		break;
	}
}

void CopyProgramControls::turnWheel(int i)
{
	init();
	auto pc = sampler.lock()->getProgramCount();
	if (param.compare("pgm0") == 0) {
		swGui->setPgm0(swGui->getPgm0() + i, pc);
	}
	else if (param.compare("pgm1") == 0) {
		swGui->setPgm1(swGui->getPgm1() + i, pc);
	}
}
