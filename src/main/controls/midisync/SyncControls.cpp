#include <controls/midisync/SyncControls.hpp>

#include <ui/midisync/MidiSyncGui.hpp>

using namespace mpc::controls::midisync;
using namespace std;

SyncControls::SyncControls(mpc::Mpc* mpc) 
	: AbstractMidiSyncControls(mpc)
{
}

void SyncControls::turnWheel(int i)
{
	init();
	if (param.compare("in") == 0) {
		midiSyncGui->setIn(midiSyncGui->getIn() + i);
	}
	else if (param.compare("out") == 0) {
		midiSyncGui->setOut(midiSyncGui->getOut() + i);
	}
	else if (param.compare("modein") == 0) {
		midiSyncGui->setModeIn(midiSyncGui->getModeIn() + i);
	}
	else if (param.compare("modeout") == 0) {
		midiSyncGui->setModeOut(midiSyncGui->getModeOut() + i);
	}
	else if (param.compare("receivemmc") == 0) {
		midiSyncGui->setReceiveMMCEnabled(i > 0);
	}
	else if (param.compare("sendmmc") == 0) {
		midiSyncGui->setSendMMCEnabled(i > 0);
	}
}

void SyncControls::function(int i)
{
	init();
	switch (i) {
	case 1:
		break;
	case 2:
		break;
	case 3:
		break;
	}
}

SyncControls::~SyncControls() {
}
