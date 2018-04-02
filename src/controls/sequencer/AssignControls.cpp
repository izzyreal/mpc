#include <controls/sequencer/AssignControls.hpp>

#include <ui/sampler/SamplerGui.hpp>
#include <sampler/Program.hpp>
#include <sampler/PgmSlider.hpp>

using namespace mpc::controls::sequencer;
using namespace std;

AssignControls::AssignControls(mpc::Mpc* mpc)
	: AbstractSequencerControls(mpc)
{
}

void AssignControls::turnWheel(int i)
{
	init();
	auto slider = program.lock()->getSlider();
	auto parameter = slider->getParameter();

	if (param.compare("assignnote") == 0) {
		slider->setAssignNote(slider->getNote() + i);
	}
	else if (param.compare("parameter") == 0) {
		slider->setParameter(slider->getParameter() + i);
	}
	else if (param.compare("highrange") == 0) {
		switch (parameter) {
		case 0:
			slider->setTuneHighRange(slider->getTuneHighRange() + i);
			break;
		case 1:
			slider->setDecayHighRange(slider->getDecayHighRange() + i);
			break;
		case 2:
			slider->setAttackHighRange(slider->getAttackHighRange() + i);
			break;
		case 3:
			slider->setFilterHighRange(slider->getFilterHighRange() + i);
			break;
		}
	}
	else if (param.compare("lowrange") == 0) {
		switch (parameter) {
		case 0:
			slider->setTuneLowRange(slider->getTuneLowRange() + i);
			break;
		case 1:
			slider->setDecayLowRange(slider->getDecayLowRange() + i);
			break;
		case 2:
			slider->setAttackLowRange(slider->getAttackLowRange() + i);
			break;
		case 3:
			slider->setFilterLowRange(slider->getFilterLowRange() + i);
			break;
		}

	}
	else if (param.compare("assignnv") == 0) {
		slider->setControlChange(slider->getControlChange() + i);
	}
}

void AssignControls::pad(int i, int velo, bool repeat, int tick)
{
    super::pad(i, velo, repeat, 0);
	auto lProgram = program.lock();
	auto nn = lProgram->getNoteFromPad(i + (samplerGui->getBank() * 16));
    lProgram->getSlider()->setAssignNote(nn);
}