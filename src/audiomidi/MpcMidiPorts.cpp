#include <audiomidi/MpcMidiPorts.hpp>

#include <Mpc.hpp>
#include <audiomidi/MpcMidiInput.hpp>

#include <midi/core/MidiMessage.hpp>
#include <midi/core/ShortMessage.hpp>

using namespace mpc::audiomidi;
using namespace std;

MpcMidiPorts::MpcMidiPorts(mpc::Mpc* mpc)
{
	this->mpc = mpc;
	receivers = std::vector<std::vector<ctoot::midi::core::ShortMessage>>(2);
	/*
	for (auto& m : cms->getMidiInputs()) {
		if (m->getName().find("MpcMultiSynth") == string::npos) {
			midiInputs.push_back(m);
		}
	}

	for (auto& m : cms->getMidiOutputs()) {
		midiOutputs.push_back(m);
	}
	*/
}

/*
vector<ctoot::midi::core::MidiOutput*> MpcMidiPorts::getTransmitters()
{
    return midiOutputs;
}

vector<ctoot::midi::core::MidiInput*> MpcMidiPorts::getReceivers()
{
    return midiInputs;
}
*/

std::vector<std::vector<ctoot::midi::core::ShortMessage> >* MpcMidiPorts::getReceivers() {
	return &receivers;
}

void MpcMidiPorts::setMidiIn1(int i)
{
	/*
	int size = midiOutputs.size();
	if (i > size - 1) return;

	if (midiIn1 != nullptr) {
		midiIn1->removeConnectionTo(mpc->getMpcMidiInput(0));
	}
	if (i < 0) {
		midiIn1 = nullptr;
		return;
	}
	midiIn1 = midiOutputs[i];
	midiIn1->addConnectionTo(mpc->getMpcMidiInput(0));
	*/
}

void MpcMidiPorts::setMidiIn2(int i)
{
	/*
	int size = midiOutputs.size();
	if (i > size - 1) return;

	if (midiIn2 != nullptr)
		midiIn2->removeConnectionTo(mpc->getMpcMidiInput(1));

	if (i < 0) {
		midiIn2 = nullptr;
		return;
	}
	midiIn2 = midiOutputs[i];
	midiIn2->addConnectionTo(mpc->getMpcMidiInput(1));
	*/
}

void MpcMidiPorts::setMidiOutA(int i)
{
	/*
	int size = midiInputs.size();
	if (i > size - 1) return;

	if (i < 0) {
		midiOutA = nullptr;
		return;
	}
	midiOutA = midiInputs[i];
	*/
}

void MpcMidiPorts::setMidiOutB(int i)
{
	/*
	int size = midiInputs.size();
	if (i > size - 1) return;

	if (i < 0) {
		midiOutB = nullptr;
		return;
	}
	midiOutB = midiInputs[i];
	*/
}

string MpcMidiPorts::getCurrentMidiIn1DeviceName()
{
	//if (midiIn1 == nullptr)
	return "NOT CONNECTED";
//    return midiIn1->getName();
}

int MpcMidiPorts::getCurrentMidiIn1DevIndex() {
	/*
	if (midiIn1 == nullptr) return -1;
	for (int i = 0; i < midiInputs.size(); i++) {
		if (midiInputs.at(i)->getName().compare(midiIn1->getName()) == 0) {
			return i;
		}
	}
	*/
    return -1;
}

string MpcMidiPorts::getCurrentMidiIn2DeviceName()
{
	//if (midiIn2 == nullptr) return "NOT CONNECTED";
	//return midiIn2->getName();
	return "NOT CONNECTED";
}

int MpcMidiPorts::getCurrentMidiIn2DevIndex() {
	/*
	if (midiIn2 == nullptr) return -1;
	for (int i = 0; i < midiInputs.size(); i++) {
		if (midiInputs.at(i)->getName().compare(midiIn2->getName()) == 0) {
			return i;
		}
	}
	*/
    return -1;
}

string MpcMidiPorts::getCurrentMidiOutADeviceName()
{
	//if (midiOutA == nullptr) return "NOT CONNECTED";
    //return midiOutA->getName();
	return "NOT CONNECTED";
}

int MpcMidiPorts::getCurrentMidiOutADevIndex() {
	/*
	if (midiOutA == nullptr) return -1;
	for (int i = 0; i < midiOutputs.size(); i++) {
		if (midiOutputs.at(i)->getName().compare(midiOutA->getName()) == 0) {
			return i;
		}
	}
	*/
    return -1;
}

string MpcMidiPorts::getCurrentMidiOutBDeviceName()
{
	//if (midiOutB == nullptr) return "NOT CONNECTED";
	//return midiOutB->getName();
	return "NOT CONNECTED";
}

int MpcMidiPorts::getCurrentMidiOutBDevIndex() {
	/*
	if (midiOutB == nullptr) return -1;
	for (int i = 0; i < midiOutputs.size(); i++) {
		if (midiOutputs.at(i)->getName().compare(midiOutB->getName()) == 0) {
			return i;
		}
	}
	*/
    return -1;
}

void MpcMidiPorts::close()
{
}

void MpcMidiPorts::transmitA(ctoot::midi::core::ShortMessage* msg)
{
	//if (midiOutA == nullptr) return;
	//midiOutA->transport(msg, 0);
}

void MpcMidiPorts::transmitB(ctoot::midi::core::ShortMessage* msg)
{
	//if (midiOutB == nullptr) return;
	//midiOutB->transport(msg, 0);
}

MpcMidiPorts::~MpcMidiPorts() {
	//midiInputs.clear();
	//midiOutputs.clear();
}
