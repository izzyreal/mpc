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


UserObserver::~UserObserver()
{
	ud.deleteObserver(this);
	timeSig.deleteObserver(this);
}
