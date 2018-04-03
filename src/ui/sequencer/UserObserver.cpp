#include <ui/sequencer/UserObserver.hpp>

#include <Mpc.hpp>
#include <Util.hpp>
#include <StartUp.hpp>
#include <lcdgui/LayeredScreen.hpp>
#include <ui/UserDefaults.hpp>
#include <lcdgui/Field.hpp>
#include <sampler/Program.hpp>
#include <sampler/Sampler.hpp>
#include <sequencer/TimeSignature.hpp>
#include <lcdgui/Label.hpp>

#include <cmath>

#include <thirdp/bcmath/bcmath_stl.h>

using namespace mpc::ui::sequencer;
using namespace std;

UserObserver::UserObserver(mpc::Mpc* mpc)
{
	busNames = { "MIDI", "DRUM1", "DRUM2", "DRUM3", "DRUM4" };
	this->mpc = mpc;
	ud = mpc::StartUp::getUserDefaults();
	auto lUd = ud.lock();
	timeSig = lUd->getTimeSig();
	timeSig->deleteObservers();
	timeSig->addObserver(this);
	lUd->deleteObservers();
	lUd->addObserver(this);
	auto ls = mpc->getLayeredScreen().lock();
	tempoField = ls->lookupField("tempo");
	tempoField.lock()->setSize(28, 9);
	loopField = ls->lookupField("loop");
	tsigField = ls->lookupField("tsig");
	barsField = ls->lookupField("bars");
	pgmField = ls->lookupField("pgm");
	recordingModeField = ls->lookupField("recordingmode");
	busField = ls->lookupField("tracktype");
	deviceNumberField = ls->lookupField("devicenumber");
	deviceNameLabel = ls->lookupLabel("devicename");
	veloField = ls->lookupField("velo");
	displayTempo();
	displayLoop();
	displayTsig();
	displayBars();
	displayPgm();
	displayRecordingMode();
	displayBus();
	displayDeviceNumber();
	displayVelo();
}

void UserObserver::displayTempo()
{
	auto tempo = ud.lock()->getTempo().toString();
	tempo = moduru::lang::StrUtil::padLeft(tempo, " ", 5);
	tempo = Util::replaceDotWithSmallSpaceDot(tempo);
	tempoField.lock()->setText(tempo);
}

void UserObserver::displayLoop()
{
	auto lUd = ud.lock();
    loopField.lock()->setText(lUd->isLoopEnabled() ? "ON" : "OFF");
}

void UserObserver::displayTsig()
{
	auto lUd = ud.lock(); 
	tsigField.lock()->setText(to_string(lUd->getTimeSig()->getNumerator()) + "/" + to_string(lUd->getTimeSig()->getDenominator()));
}

void UserObserver::displayBars()
{
	auto lUd = ud.lock(); 
	barsField.lock()->setText(to_string(lUd->getLastBarIndex() + 1));
}

void UserObserver::displayPgm()
{
	auto lUd = ud.lock();
	if(lUd->getPgm() == 0) {
        pgmField.lock()->setText("OFF");
    } else {
        pgmField.lock()->setText(to_string(lUd->getPgm()));
    }
}

void UserObserver::displayRecordingMode()
{
	auto lUd = ud.lock();
	recordingModeField.lock()->setText(lUd->isRecordingModeMulti() ? "M" : "S");
}

void UserObserver::displayBus()
{
	auto lUd = ud.lock();
	busField.lock()->setText(busNames[lUd->getBus()]);
    displayDeviceName();
}

void UserObserver::displayDeviceNumber()
{
	auto lUd = ud.lock();
	if (lUd->getDeviceNumber() == 0) {
		deviceNumberField.lock()->setText("OFF");
	}
	else {
		if (lUd->getDeviceNumber() >= 17) {
			deviceNumberField.lock()->setTextPadded(to_string(lUd->getDeviceNumber() - 16) + "B", " ");
		}
		else {
			deviceNumberField.lock()->setTextPadded(to_string(lUd->getDeviceNumber()) + "A", " ");
		}
	}
}

void UserObserver::displayVelo()
{
	auto lUd = ud.lock();
	veloField.lock()->setText(to_string(lUd->getVeloRatio()));
}

void UserObserver::update(moduru::observer::Observable* o, boost::any arg)
{
	string s = boost::any_cast<string>(arg);
	if (s.compare("tempo") == 0) {
		displayTempo();
	}
	else if (s.compare("loop") == 0) {
		displayLoop();
	}
	else if (s.compare("timesignature") == 0) {
		displayTsig();
	}
	else if (s.compare("bars") == 0) {
		displayBars();
	}
	else if (s.compare("pgm") == 0) {
		displayPgm();
	}
	else if (s.compare("recordingmode") == 0) {
		displayRecordingMode();
	}
	else if (s.compare("tracktype") == 0) {
		displayBus();
	}
	else if (s.compare("devicenumber") == 0) {
		displayDeviceNumber();
		displayDeviceName();
	}
	else if (s.compare("velo") == 0) {
		displayVelo();
	}
}

void UserObserver::displayDeviceName()
{
	auto sampler = mpc->getSampler().lock();
	auto lUd = ud.lock();
	if (lUd->getBus() != 0) {
		if (lUd->getDeviceNumber() == 0) {
			auto p = dynamic_pointer_cast<mpc::sampler::Program>(sampler->getProgram(sampler->getDrumBusProgramNumber(lUd->getBus())).lock());
			deviceNameLabel.lock()->setText(p->getName());
		}
		else {
			deviceNameLabel.lock()->setText(lUd->getDeviceName(lUd->getDeviceNumber()));
		}
	}
	if (lUd->getBus() == 0) {
		if (lUd->getDeviceNumber() != 0) {
			deviceNameLabel.lock()->setText(lUd->getDeviceName(lUd->getDeviceNumber()));
		}
		else {
			deviceNameLabel.lock()->setText("");
		}
	}
}

UserObserver::~UserObserver() {
	ud.lock()->deleteObserver(this);
}
