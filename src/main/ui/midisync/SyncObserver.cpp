#include "SyncObserver.hpp"

#include <ui/Uis.hpp>
#include <ui/midisync/MidiSyncGui.hpp>

#include <lcdgui/LayeredScreen.hpp>
#include <lcdgui/Field.hpp>

using namespace std;
using namespace mpc::ui::midisync;

SyncObserver::SyncObserver(mpc::Mpc* mpc)
{
	modeNames = vector<string>{ "OFF", "MIDI CLOCK", "TIME CODE"	};
	msGui = mpc->getUis().lock()->getMidiSyncGui();
	msGui->deleteObservers();
	msGui->addObserver(this);
	auto ls = mpc->getLayeredScreen().lock();
	inField = ls->lookupField("in");
	outField = ls->lookupField("out");
	modeInField = ls->lookupField("modein");
	modeOutField = ls->lookupField("modeout");
	receiveMMCField = ls->lookupField("receivemmc");
	sendMMCField = ls->lookupField("sendmmc");
	displayIn();
	displayOut();
	displayModeIn();
	displayModeOut();
	displayReceiveMMC();
	displaySendMMC();
}

void SyncObserver::displayIn()
{
	string result = to_string(msGui->getIn() + 1);
	inField.lock()->setText(result);
}

void SyncObserver::displayOut()
{
    auto out = " A";
    if(msGui->getOut() == 1)
        out = " B";

    if(msGui->getOut() == 2)
        out = "A/B";

    outField.lock()->setText(out);
}

void SyncObserver::displayModeIn()
{
    modeInField.lock()->setText(modeNames[msGui->getModeIn()]);
}

void SyncObserver::displayModeOut()
{
    modeOutField.lock()->setText(modeNames[msGui->getModeOut()]);
}

void SyncObserver::displayReceiveMMC()
{
    auto mmc = msGui->isReceiveMMCEnabled() ? "ON" : "OFF";
    receiveMMCField.lock()->setText(mmc);
}

void SyncObserver::displaySendMMC()
{
    auto mmc = msGui->isSendMMCEnabled() ? "ON" : "OFF";
    sendMMCField.lock()->setText(mmc);
}

void SyncObserver::update(moduru::observer::Observable* o, nonstd::any arg)
{
	string s = nonstd::any_cast<string>(arg);
	if (s.compare("in") == 0) {
		displayIn();
	}
	else if (s.compare("out") == 0) {
		displayOut();
	}
	else if (s.compare("modein") == 0) {
		displayModeIn();
	}
	else if (s.compare("modeout") == 0) {
		displayModeOut();
	}
	else if (s.compare("receivemmc") == 0) {
		displayReceiveMMC();
	}
	else if (s.compare("sendmcc") == 0) {
		displaySendMMC();
	}
}

SyncObserver::~SyncObserver() {
}
