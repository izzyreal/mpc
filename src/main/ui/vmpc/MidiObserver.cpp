#include <ui/vmpc/MidiObserver.hpp>

#include <Mpc.hpp>
//#include <audiomidi/MpcMidiPorts.hpp>
#include <lcdgui/Field.hpp>
#include <ui/vmpc/MidiGui.hpp>

using namespace mpc::ui::vmpc;
using namespace std;

MidiObserver::MidiObserver(mpc::Mpc* mpc)
{
	inNames = { "MIDI In 1", "MIDI In 2" };
	outNames = { "MIDI OUT A", "MIDI OUT B" };
	//this->mpcMidiPorts = mpc->getMidiPorts().lock().get();
	midiGui = mpc->getUis().lock()->getMidiGui();
	midiGui->addObserver(this);
	auto ls = mpc->getLayeredScreen().lock();
	dev0Field = ls->lookupField("dev0");
	dev1Field = ls->lookupField("dev1");
	inField = ls->lookupField("in");
	outField = ls->lookupField("out");
	displayIn();
	displayDev0();
	displayOut();
	displayDev1();
}

void MidiObserver::displayDev0()
{
	if (midiGui->getIn() == 0) {
		//dev0Field.lock()->setText(mpcMidiPorts->getCurrentMidiIn1DeviceName());
	}
	else if (midiGui->getIn() == 1) {
		//dev0Field.lock()->setText(mpcMidiPorts->getCurrentMidiIn2DeviceName());
	}
}

void MidiObserver::displayIn()
{
	inField.lock()->setText(inNames[midiGui->getIn()]);
	displayDev0();
}

void MidiObserver::displayDev1()
{
	if (midiGui->getOut() == 0) {
		//dev1Field.lock()->setText(mpcMidiPorts->getCurrentMidiOutADeviceName());
	}
	else if (midiGui->getOut() == 1) {
		//dev1Field.lock()->setText(mpcMidiPorts->getCurrentMidiOutBDeviceName());
	}
}

void MidiObserver::displayOut()
{
	outField.lock()->setText(outNames[midiGui->getOut()]);
	displayDev1();
}

void MidiObserver::update(moduru::observer::Observable* o, nonstd::any a)
{
	string param = nonstd::any_cast<string>(a);
	if (param.compare("in") == 0) {
		displayIn();
	}
	else if (param.compare("out") == 0) {
		displayOut();
	}
	else if (param.compare("dev0") == 0) {
		displayDev0();
	}
	else if (param.compare("dev1") == 0) {
		displayDev1();
	}
}

MidiObserver::~MidiObserver() {
	midiGui->deleteObserver(this);
}
