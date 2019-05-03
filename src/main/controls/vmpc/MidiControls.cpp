#include <controls/vmpc/MidiControls.hpp>

//#include <audiomidi/MpcMidiPorts.hpp>
#include <ui/vmpc/MidiGui.hpp>

using namespace mpc::controls::vmpc;
using namespace std;

MidiControls::MidiControls(mpc::Mpc* mpc)
	: AbstractVmpcControls(mpc)
{
}

void MidiControls::up() {
	init();
	if (param.compare("dev1") == 0) {
		ls.lock()->setFocus("out");
	}
	else if (param.compare("out") == 0) {
		ls.lock()->setFocus("dev0");
	}
	else super::up();
}

void MidiControls::down() {
	init();
	if (param.compare("in") == 0) {
		ls.lock()->setFocus("dev0");
	}
	else if (param.compare("dev0") == 0) {
		ls.lock()->setFocus("out");
	}
	else super::down();
}

void MidiControls::function(int i)
{
    init();
	switch (i) {
    case 0:
        ls.lock()->openScreen("audio");
        break;
    case 2:
        ls.lock()->openScreen("disk");
        break;
    }
}

void MidiControls::turnWheel(int i)
{
	init();
	if (param.compare("in") == 0) {
		midiGui->setIn(midiGui->getIn() + i);
	}
	else if (param.compare("dev0") == 0) {
		if (midiGui->getIn() == 0) {
			auto prevTransmitterNumber = midiGui->getIn1TransmitterNumber();
			//auto candidate = prevTransmitterNumber + i;
			//midiGui->setIn1TransmitterNumber(candidate, mpcMidiPorts->getTransmitters().size()-1);
			auto newTransmitterNumber = midiGui->getIn1TransmitterNumber();
			if (newTransmitterNumber != prevTransmitterNumber) {
				//mpcMidiPorts->setMidiIn1(newTransmitterNumber);
				midiGui->setChanged();
				midiGui->notifyObservers(param);
			}
		}
		else if (midiGui->getIn() == 1) {
			auto prevTransmitterNumber = midiGui->getIn2TransmitterNumber();
			//auto candidate = prevTransmitterNumber + i;
			//midiGui->setIn2TransmitterNumber(candidate, mpcMidiPorts->getTransmitters().size() - 1);
			auto newTransmitterNumber = midiGui->getIn2TransmitterNumber();
			if (newTransmitterNumber != prevTransmitterNumber) {
				//mpcMidiPorts->setMidiIn2(newTransmitterNumber);
				midiGui->setChanged();
				midiGui->notifyObservers(param);
			}
		}
	}
	else if (param.compare("out") == 0) {
		midiGui->setOut(midiGui->getOut() + i);
	}
	else if (param.compare("dev1") == 0) {
		if (midiGui->getOut() == 0) {
			auto prevReceiverNumber = midiGui->getOutAReceiverIndex();
			//auto candidate = prevReceiverNumber + i;
			//midiGui->setOutAReceiverNumber(candidate, mpcMidiPorts->getReceivers().size() - 1);
			auto newReceiverNumber = midiGui->getOutAReceiverIndex();
			if (newReceiverNumber != prevReceiverNumber) {
				//mpcMidiPorts->setMidiOutA(newReceiverNumber);
				midiGui->setChanged();
				midiGui->notifyObservers(param);
			}
		}
		else if (midiGui->getOut() == 1) {
			auto prevReceiverNumber = midiGui->getOutBReceiverIndex();
			//auto candidate = prevReceiverNumber + i;
			//midiGui->setOutBReceiverNumber(candidate, mpcMidiPorts->getReceivers().size() - 1);
			auto newReceiverNumber = midiGui->getOutBReceiverIndex();
			if (newReceiverNumber != prevReceiverNumber) {
				//mpcMidiPorts->setMidiOutB(newReceiverNumber);
				midiGui->setChanged();
				midiGui->notifyObservers(param);
			}
		}
	}
}
