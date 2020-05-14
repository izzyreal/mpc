#include <controls/sampler/dialog/DeleteProgramControls.hpp>

#include <ui/sampler/window/SamplerWindowGui.hpp>
#include <sampler/Program.hpp>
#include <sampler/Sampler.hpp>

using namespace mpc::controls::sampler::dialog;
using namespace std;

DeleteProgramControls::DeleteProgramControls() 
	: AbstractSamplerControls()
{
}

void DeleteProgramControls::function(int i)
{
    init();
	
	auto lLs = ls.lock();
	switch (i) {
    case 2:
        lLs->openScreen("deleteallprograms");
        break;
    case 3:
        lLs->openScreen("program");
        break;
    case 4:
        if (sampler.lock()->getProgramCount() > 1) {
			sampler.lock()->deleteProgram(dynamic_pointer_cast<mpc::sampler::Program>(sampler.lock()->getProgram(swGui->getDeletePgm()).lock()));
        } else {
			const bool initPgms = true;
			sampler.lock()->deleteAllPrograms(initPgms);
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
