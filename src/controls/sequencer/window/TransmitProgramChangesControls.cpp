#include <controls/sequencer/window/TransmitProgramChangesControls.hpp>

#include <ui/sequencer/window/SequencerWindowGui.hpp>

using namespace mpc::controls::sequencer::window;
using namespace std;

TransmitProgramChangesControls::TransmitProgramChangesControls(mpc::Mpc* mpc)
	: AbstractSequencerControls(mpc)
{
}

void TransmitProgramChangesControls::turnWheel(int i)
{
	init();
	if (param.compare("inthistrack") == 0) {
		swGui->setTransmitProgramChangesInThisTrack(i > 0);
	}
}
