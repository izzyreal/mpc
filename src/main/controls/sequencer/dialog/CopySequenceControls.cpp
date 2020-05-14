#include "CopySequenceControls.hpp"

#include <ui/sequencer/window/SequencerWindowGui.hpp>
#include <sequencer/Sequencer.hpp>

using namespace mpc::controls::sequencer::dialog;
using namespace std;

CopySequenceControls::CopySequenceControls()
	: AbstractSequencerControls()
{
}

void CopySequenceControls::function(int i)
{
	init();
	
	switch (i) {
	case 2:
		sequencer.lock()->copySequence(swGui->getSq0(), swGui->getSq1());
		ls.lock()->openScreen("sequencer");
		break;
	case 4:
		sequencer.lock()->copySequence(swGui->getSq0(), swGui->getSq1());
		ls.lock()->openScreen("sequencer");
		break;
	}
}

void CopySequenceControls::turnWheel(int i)
{
    init();
	if (param.find("0") != string::npos) {
		swGui->setSq0(swGui->getSq0() + i);
	}
	else if (param.find("1") != string::npos) {
		swGui->setSq1(swGui->getSq1() + i);
	}
}
