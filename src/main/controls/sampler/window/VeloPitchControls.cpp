#include <controls/sampler/window/VeloPitchControls.hpp>

#include <ui/sampler/SamplerGui.hpp>
#include <sampler/NoteParameters.hpp>

using namespace mpc::controls::sampler::window;
using namespace std;

VeloPitchControls::VeloPitchControls()
	: AbstractSamplerControls()
{
}

void VeloPitchControls::turnWheel(int i)
{
	init();
	if (param.compare("tune") == 0) {
		lastNp->setTune(lastNp->getTune() + i);
	}
	else if (param.compare("velopitch") == 0) {
		lastNp->setVelocityToPitch(lastNp->getVelocityToPitch() + i);
	}
	else if (param.compare("note") == 0) {
		samplerGui->setPadAndNote(samplerGui->getPad(), samplerGui->getNote() + i);
	}
}
