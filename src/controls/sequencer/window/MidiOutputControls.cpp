#include "MidiOutputControls.hpp"

#include <lcdgui/LayeredScreen.hpp>
#include <ui/NameGui.hpp>
#include <ui/sequencer/window/SequencerWindowGui.hpp>
#include <sequencer/Sequence.hpp>

using namespace mpc::controls::sequencer::window;
using namespace std;

MidiOutputControls::MidiOutputControls(mpc::Mpc* mpc)
	: AbstractSequencerControls(mpc)
{
}

void MidiOutputControls::turnWheel(int i)
{
	init();
	auto seq = sequence.lock();
	if (param.compare("firstletter") == 0) {
		nameGui->setName(seq->getDeviceName(swGui->getDeviceNumber() + i));
		nameGui->setParameterName("devicename");
		nameGui->setNameLimit(8);
		ls.lock()->openScreen("name");
	}
	else if (param.compare("softthru") == 0) {
		swGui->setSoftThru(swGui->getSoftThru() + i);
	}
	else if (param.compare("devicenumber") == 0) {
		swGui->setDeviceNumber(swGui->getDeviceNumber() + i);
	}
}

void MidiOutputControls::function(int i)
{
	super::function(i);
	switch (i) {
	case 1:
		ls.lock()->openScreen("midioutputmonitor");
		break;
	}
}
