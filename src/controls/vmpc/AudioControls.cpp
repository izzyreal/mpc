#include <controls/vmpc/AudioControls.hpp>

#include <audiomidi/AudioMidiServices.hpp>
//#include <audiomidi/MpcMidiPorts.hpp>
#include <ui/vmpc/AudioGui.hpp>
#include <ui/vmpc/MidiGui.hpp>

#include <thread>
#include <chrono>

using namespace mpc::controls::vmpc;
using namespace std;

mpc::controls::vmpc::AudioControls::AudioControls(mpc::Mpc* mpc)
	: AbstractVmpcControls(mpc)
{
}

void mpc::controls::vmpc::AudioControls::openWindow()
{
    init();
	auto lAms = ams.lock();
	//if (lAms->isStandalone()) {
		ls.lock()->openScreen("buffersize");
	//}
}

void mpc::controls::vmpc::AudioControls::turnWheel(int i)
{
	init();

	if (param.compare("server") == 0) {
		//if (!ams.lock()->isStandalone()) return;
		audioGui->setServer(audioGui->getServer() + i);
	}
	else if (param.compare("in") == 0) {
		audioGui->setIn(audioGui->getIn() + i);
	}
	else if (param.compare("out") == 0) {
		audioGui->setOut(audioGui->getOut() + i);
	}
	else if (param.compare("dev0") == 0) {
		//if (ams.lock()->getServerIndex() != audioGui->getServer()) return;
		audioGui->setDev0(audioGui->getDev0() + i);
	}
	else if (param.compare("dev1") == 0) {
		//if (ams.lock()->getServerIndex() != audioGui->getServer()) return;
		audioGui->setDev1(audioGui->getDev1() + i);
	}
}

void mpc::controls::vmpc::AudioControls::function(int i)
{
	init();
	auto lAms = ams.lock();
	switch (i) {
	case 1:
	{
		//midiGui->setIn1TransmitterNumber(mpcMidiPorts->getCurrentMidiIn1DevIndex(), mpcMidiPorts->getTransmitters().size() - 1);
		//midiGui->setIn2TransmitterNumber(mpcMidiPorts->getCurrentMidiIn2DevIndex(), mpcMidiPorts->getTransmitters().size() - 1);
		//midiGui->setOutAReceiverNumber(mpcMidiPorts->getCurrentMidiOutADevIndex(), mpcMidiPorts->getReceivers().size() - 1);
		//midiGui->setOutBReceiverNumber(mpcMidiPorts->getCurrentMidiOutBDevIndex(), mpcMidiPorts->getReceivers().size() - 1);
		ls.lock()->openScreen("midi");
		break;
	}
	case 2:
		ls.lock()->openScreen("disk");
		break;
	case 5:
		/*
		int oldServer = lAms->getServerIndex();
		int newServer = audioGui->getServer();
		
		bool ioChanged = false;
		if (newServer == oldServer) {
			for (int i = 0; i < 2; i++) {
				if (lAms->getSelectedInputs()[i] != audioGui->getInputDevs()[i]) {
					ioChanged = true;
					lAms->setSelectedInput(i, audioGui->getInputDevs()[i]);
				}
			}
			for (int i = 0; i < 5; i++) {
				if (lAms->getSelectedOutputs()[i] != audioGui->getOutputDevs()[i]) {
					ioChanged = true;
					lAms->setSelectedOutput(i, audioGui->getOutputDevs()[i]);
				}
			}
		}
		else {
			for (int i = 0; i < 2; i++) {
				lAms->setSelectedInput(i, -1);
			}
			for (int i = 0; i < 5; i++) {
				lAms->setSelectedOutput(i, -1);
			}
			lAms->setSelectedOutput(0, 0);
			audioGui->setInputDevs(vector<int>{-1, -1});
			audioGui->setOutputDevs(vector<int>{0, -1, -1, -1, -1});
		}
		
		if (newServer != oldServer || ioChanged) {
			lAms->setDriver(newServer);
			if (oldServer == lAms->getServerIndex()) {
				//lMainFrame->createPopup("DEVICE ERROR!", 170);
				//std::this_thread::sleep_for(chrono::seconds(1));
				//lMainFrame->removePopup();
			}
			else {
				audioGui->setChanged();
				audioGui->notifyObservers(string("server"));
				//lMainFrame->createPopup("Driver changed successfully.", 170);
				//std::this_thread::sleep_for(chrono::seconds(500));
				//lMainFrame->removePopup();
			}
		}
		*/
		break;
	}
}

AudioControls::~AudioControls() {
}
