#include <controls/sampler/PurgeControls.hpp>

#include <sampler/Sampler.hpp>

using namespace mpc::controls::sampler;
using namespace std;

PurgeControls::PurgeControls() 
	: AbstractSamplerControls()
{
}

void PurgeControls::function(int f)
{
	init();
	
	auto lLs = ls.lock();
	switch (f) {
	case 0:
		lLs->openScreen("programassign");
		break;
	case 1:
		lLs->openScreen("programparams");
		break;
	case 2:
		lLs->openScreen("drum");
		break;
	case 3:
		lLs->openScreen("selectdrum");
		break;
	case 4:
		sampler.lock()->purge();
		lLs->openScreen("purge");
		break;
	}
}
