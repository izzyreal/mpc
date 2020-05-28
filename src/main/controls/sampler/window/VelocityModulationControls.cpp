#include <controls/sampler/window/VelocityModulationControls.hpp>

#include <sampler/NoteParameters.hpp>

using namespace mpc::controls::sampler::window;
using namespace std;

VelocityModulationControls::VelocityModulationControls()
	: AbstractSamplerControls()
{
}

void VelocityModulationControls::turnWheel(int i)
{
    init();
    if (param.compare("veloattack") == 0) {
		lastNp->setVelocityToAttack(lastNp->getVelocityToAttack() + i);
	}
	else if (param.compare("velostart") == 0) {
		lastNp->setVelocityToStart(lastNp->getVelocityToStart() + i);
	}
	else if (param.compare("velolevel") == 0) {
		lastNp->setVeloToLevel(lastNp->getVeloToLevel() + i);
	}
	else if (param.compare("note") == 0) {
		mpc.setPadAndNote(mpc.getPad(), mpc.getNote() + i);
	}
}