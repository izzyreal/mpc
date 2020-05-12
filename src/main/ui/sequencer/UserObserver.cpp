#include "UserObserver.hpp"

#include <Mpc.hpp>
#include <Util.hpp>
#include <lcdgui/LayeredScreen.hpp>
#include <ui/UserDefaults.hpp>
#include <lcdgui/Field.hpp>
#include <sampler/Program.hpp>
#include <sampler/Sampler.hpp>
#include <lcdgui/Label.hpp>

#include <cmath>

#include <thirdp/bcmath/bcmath_stl.h>

using namespace mpc::ui::sequencer;
using namespace std;

UserObserver::UserObserver()
	: timeSig(mpc::ui::UserDefaults::instance().getTimeSig()), ud(mpc::ui::UserDefaults::instance())
{
	busNames = { "MIDI", "DRUM1", "DRUM2", "DRUM3", "DRUM4" };
		
	timeSig.addObserver(this);
	
	ud.addObserver(this);

	auto ls = Mpc::instance().getLayeredScreen().lock();
	
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
	auto tempo = ud.getTempo().toString();
	tempo = moduru::lang::StrUtil::padLeft(tempo, " ", 5);
	tempo = Util::replaceDotWithSmallSpaceDot(tempo);
	tempoField.lock()->setText(tempo);
}

void UserObserver::displayLoop()
{
    loopField.lock()->setText(ud.isLoopEnabled() ? "ON" : "OFF");
}

void UserObserver::displayTsig()
{
	tsigField.lock()->setText(to_string(timeSig.getNumerator()) + "/" + to_string(timeSig.getDenominator()));
}

void UserObserver::displayBars()
{
	barsField.lock()->setText(to_string(ud.getLastBarIndex() + 1));
}

void UserObserver::displayPgm()
{
	if(ud.getPgm() == 0)
	{
        pgmField.lock()->setText("OFF");
    }
	else
	{
        pgmField.lock()->setText(to_string(ud.getPgm()));
    }
}

void UserObserver::displayRecordingMode()
{
	recordingModeField.lock()->setText(ud.isRecordingModeMulti() ? "M" : "S");
}

void UserObserver::displayBus()
{
	busField.lock()->setText(busNames[ud.getBus()]);
    displayDeviceName();
}

void UserObserver::displayDeviceNumber()
{
	if (ud.getDeviceNumber() == 0)
	{
		deviceNumberField.lock()->setText("OFF");
	}
	else
	{
		if (ud.getDeviceNumber() >= 17)
		{
			deviceNumberField.lock()->setTextPadded(to_string(ud.getDeviceNumber() - 16) + "B", " ");
		}
		else
		{
			deviceNumberField.lock()->setTextPadded(to_string(ud.getDeviceNumber()) + "A", " ");
		}
	}
}

void UserObserver::displayVelo()
{
	veloField.lock()->setText(to_string(ud.getVeloRatio()));
}

void UserObserver::update(moduru::observer::Observable* o, nonstd::any arg)
{
	string s = nonstd::any_cast<string>(arg);
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
	auto sampler = Mpc::instance().getSampler().lock();

	if (ud.getBus() != 0)
	{
		if (ud.getDeviceNumber() == 0)
		{
			auto p = dynamic_pointer_cast<mpc::sampler::Program>(sampler->getProgram(sampler->getDrumBusProgramNumber(ud.getBus())).lock());
			deviceNameLabel.lock()->setText(p->getName());
		}
		else
		{
			deviceNameLabel.lock()->setText(ud.getDeviceName(ud.getDeviceNumber()));
		}
	}

	if (ud.getBus() == 0)
	{
		if (ud.getDeviceNumber() != 0)
		{
			deviceNameLabel.lock()->setText(ud.getDeviceName(ud.getDeviceNumber()));
		}
		else
		{
			deviceNameLabel.lock()->setText("");
		}
	}
}

UserObserver::~UserObserver()
{
	ud.deleteObserver(this);
	timeSig.deleteObserver(this);
}
