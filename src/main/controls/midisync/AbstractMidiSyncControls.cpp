#include <controls/midisync/AbstractMidiSyncControls.hpp>

#include <Mpc.hpp>
//#include <audiomidi/MpcMidiPorts.hpp>
#include <ui/midisync/MidiSyncGui.hpp>

using namespace mpc::controls::midisync;
using namespace std;

AbstractMidiSyncControls::AbstractMidiSyncControls() 
	: BaseControls()
{
	midiSyncGui = Mpc::instance().getUis().lock()->getMidiSyncGui();
	//mpcMidiPorts = Mpc::instance().getMidiPorts();
}

AbstractMidiSyncControls::~AbstractMidiSyncControls() {
}
