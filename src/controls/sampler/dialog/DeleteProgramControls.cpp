#include <controls/sampler/dialog/DeleteProgramControls.hpp>

#include <ui/sampler/window/SamplerWindowGui.hpp>
#include <sampler/Program.hpp>
#include <sampler/Sampler.hpp>

using namespace mpc::controls::sampler::dialog;
using namespace std;

DeleteProgramControls::DeleteProgramControls(mpc::Mpc* mpc) 
	: AbstractSamplerControls(mpc)
{
}

void DeleteProgramControls::function(int i)
{
    init();
	auto lSampler = sampler.lock();
	auto lLs = ls.lock();
	switch (i) {
    case 2:
        lLs->openScreen("deleteallprograms");
        break;
    case 3:
        lLs->openScreen("program");
        break;
    case 4:
        if (lSampler->getProgramCount() > 1) {
			lSampler->deleteProgram(lSampler->getProgram(swGui->getDeletePgm()));
        } else {
			const bool initPgms = true;
			lSampler->deleteAllPrograms(initPgms);
		}
        lLs->openScreen("program");
        break;
    }
}

void DeleteProgramControls::turnWheel(int i)
{
    init();
    if (param.compare("pgm") == 0) {
		swGui->setDeletePgm(swGui->getDeletePgm() + i, sampler.lock()->getProgramCount());
	}
}
