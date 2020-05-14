#include "CopyTrackControls.hpp"

#include <ui/sequencer/window/SequencerWindowGui.hpp>
#include <sequencer/Sequencer.hpp>


using namespace mpc::controls::sequencer::dialog;
using namespace std;

CopyTrackControls::CopyTrackControls()
	: AbstractSequencerControls()
{
}

void CopyTrackControls::function(int i)
{
	init();
	
	switch (i) {
	case 4:
		sequencer.lock()->copyTrack(swGui->getTr0(), swGui->getTr1(), sequencer.lock()->getActiveSequenceIndex(), sequencer.lock()->getActiveSequenceIndex());
		ls.lock()->openScreen("sequencer");
		break;
	}
}

void CopyTrackControls::turnWheel(int i)
{
	init();
	if (param.find("0") != string::npos) {
		swGui->setTr0(swGui->getTr0() + i);
	}
	else if (param.find("1") != string::npos) {
		swGui->setTr1(swGui->getTr1() + i);
	}
}
