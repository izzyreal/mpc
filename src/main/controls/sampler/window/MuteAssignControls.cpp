#include <controls/sampler/window/MuteAssignControls.hpp>

#include <ui/sampler/SamplerGui.hpp>
#include <sampler/NoteParameters.hpp>

using namespace mpc::controls::sampler::window;
using namespace std;

MuteAssignControls::MuteAssignControls()
	: AbstractSamplerControls()
{
}

void MuteAssignControls::turnWheel(int i)
{
    init();
    if (param.compare("note") == 0) {
		mpc.setPadAndNote(mpc.getPad(), mpc.getNote() + i);
	}
	else if (param.compare("note0") == 0) {
		lastNp->setMuteAssignA(lastNp->getMuteAssignA() + i);
	}
	else if (param.compare("note1") == 0) {
		lastNp->setMuteAssignB(lastNp->getMuteAssignB() + i);
	}
}
