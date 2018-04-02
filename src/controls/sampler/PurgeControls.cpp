#include <controls/sampler/PurgeControls.hpp>

#include <sampler/Sampler.hpp>

using namespace mpc::controls::sampler;
using namespace std;

PurgeControls::PurgeControls(mpc::Mpc* mpc) 
	: AbstractSamplerControls(mpc)
{
}

void PurgeControls::function(int f)
{
	init();
	auto lSampler = sampler.lock();
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
		lSampler->purge();
		lLs->openScreen("purge");
		break;
	}
}
