#include <controls/sampler/dialog/DeleteAllProgramsControls.hpp>

#include <sampler/Program.hpp>
#include <sampler/Sampler.hpp>

using namespace mpc::controls::sampler::dialog;
using namespace std;


DeleteAllProgramsControls::DeleteAllProgramsControls() 
	: AbstractSamplerControls()
{
}

void DeleteAllProgramsControls::function(int j)
{
	init();
	
	auto lLs = ls.lock();
	switch (j) {
	case 3:
		lLs->openScreen("deleteprogram");
		break;
	case 4:
		const bool initPgms = true;
		sampler.lock()->deleteAllPrograms(initPgms);
		lLs->openScreen("deleteprogram");
		break;
	}
}
