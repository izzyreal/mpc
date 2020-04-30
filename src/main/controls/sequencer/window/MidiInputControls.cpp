#include <controls/sequencer/window/MidiInputControls.hpp>

#include <lcdgui/LayeredScreen.hpp>
#include <ui/sequencer/window/SequencerWindowGui.hpp>

using namespace mpc::controls::sequencer::window;
using namespace std;

MidiInputControls::MidiInputControls()
	: AbstractSequencerControls()
{
}

void MidiInputControls::function(int i)
{
	super::function(i);
	switch (i) {
	case 1:
		ls.lock()->openScreen("midiinputmonitor");
		break;
	}
}

void MidiInputControls::turnWheel(int i)
{
	init();
	if (param.compare("receivech") == 0) {
		swGui->setReceiveCh(swGui->getReceiveCh() + i);
	}
	else if (param.compare("seq") == 0) {
		swGui->setProgChangeSeq(i > 0);
	}
	else if (param.compare("duration") == 0) {
		swGui->setSustainPedalToDuration(i > 0);
	}
	else if (param.compare("midifilter") == 0) {
		swGui->setMidiFilterEnabled(i > 0);
	}
	else if (param.compare("type") == 0) {
		swGui->setFilterType(swGui->getMidiFilterType() + i);
	}
	else if (param.compare("pass") == 0) {
		swGui->setPass(i > 0);
	}
}
