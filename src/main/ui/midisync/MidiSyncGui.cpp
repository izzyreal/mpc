#include <ui/midisync/MidiSyncGui.hpp>

using namespace mpc::ui::midisync;
using namespace std;

MidiSyncGui::MidiSyncGui()
{
	modeIns = { 0, 0 };
	modeOuts = { 0, 0 };
}

void MidiSyncGui::setIn(int i)
{
	if (i < 0 || i > 1) return;
    in = i;
    setChanged();
    notifyObservers(string("in"));
}

int MidiSyncGui::getIn()
{
    return in;
}

void MidiSyncGui::setOut(int i)
{
    if (i < 0 || i > 2) return;

    out = i;
    setChanged();
    notifyObservers(string("out"));
}

int MidiSyncGui::getOut()
{
    return out;
}

int MidiSyncGui::getShiftEarly()
{
    return shiftEarly;
}

void MidiSyncGui::setShiftEarly(int i)
{
    shiftEarly = i;
}

bool MidiSyncGui::isSendMMCEnabled()
{
    return sendMMCEnabled;
}

void MidiSyncGui::setSendMMCEnabled(bool b)
{
    sendMMCEnabled = b;
    setChanged();
    notifyObservers(string("sendmmc"));
}

int MidiSyncGui::getFrameRate()
{
    return frameRate;
}

void MidiSyncGui::setFrameRate(int i)
{
    frameRate = i;
}

int MidiSyncGui::getModeIn()
{
	return modeIns[in];
}

void MidiSyncGui::setModeIn(int i)
{
    if (i < 0 || i > 2) return;
    modeIns[in] = i;
    setChanged();
    notifyObservers(string("modein"));
}

int MidiSyncGui::getModeOut()
{
	if (out > 1) return modeOuts[0];
    return modeOuts[out];
}

void MidiSyncGui::setModeOut(int i)
{
    if (i < 0 || i > 2) return;
	if (out <= 1) {
		modeOuts[out] = i;
	}
	else {
		modeOuts[0] = i;
		modeOuts[1] = i;
	}
    setChanged();
    notifyObservers(string("modeout"));
}

bool MidiSyncGui::isReceiveMMCEnabled()
{
    return receiveMMCEnabled;
}

void MidiSyncGui::setReceiveMMCEnabled(bool b)
{
    receiveMMCEnabled = b;
    setChanged();
    notifyObservers(string("receivemmc"));
}

vector<int>* MidiSyncGui::getModeIns() {
	return &modeIns;
}
