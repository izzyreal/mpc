#include <controls/midisync/AbstractMidiSyncControls.hpp>

#include <Mpc.hpp>
//#include <audiomidi/MpcMidiPorts.hpp>
#include <ui/midisync/MidiSyncGui.hpp>

using namespace mpc::controls::midisync;
using namespace std;

AbstractMidiSyncControls::AbstractMidiSyncControls(mpc::Mpc* mpc) 
	: BaseControls(mpc)
{
	midiSyncGui = mpc->getUis().lock()->getMidiSyncGui();
	//mpcMidiPorts = mpc->getMidiPorts();
}

AbstractMidiSyncControls::~AbstractMidiSyncControls() {
}
