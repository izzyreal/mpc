#include "UserControls.hpp"

#include <lcdgui/LayeredScreen.hpp>
#include <ui/UserDefaults.hpp>
#include <sequencer/TimeSignature.hpp>

using namespace mpc::controls::sequencer;
using namespace std;

UserControls::UserControls() 
	: AbstractSequencerControls()
{
}

void UserControls::function(int i)
{
    init();
	switch (i) {
    case 0:
        ls.lock()->openScreen("edit");
        break;
    case 1:
        ls.lock()->openScreen("barcopy");
        break;
    case 2:
        ls.lock()->openScreen("trmove");
        break;
    }

}

void UserControls::turnWheel(int i)
{
	init();
	
	auto ud = mpc::ui::UserDefaults::instance();

	if (param.compare("tempo") == 0) {
		double oldTempo = ud.getTempo().toDouble();
		double newTempo = oldTempo + (i / 10.0);
		ud.setTempo(BCMath(newTempo));
	}
	else if (param.compare("loop") == 0) {
		ud.setLoop(i > 0);
	}
	else if (param.compare("tsig") == 0) {
		if (i > 0) {
			ud.getTimeSig().increase();
		}
		else {
			ud.getTimeSig().decrease();
		}
	}
	else if (param.compare("bars") == 0) {
		ud.setLastBar(ud.getLastBarIndex() + i);
	}
	else if (param.compare("pgm") == 0) {
		ud.setPgm(ud.getPgm() + i);
	}
	else if (param.compare("recordingmode") == 0) {
		ud.setRecordingModeMulti(i > 0);
	}
	else if (param.compare("tracktype") == 0) {
		ud.setBus(ud.getBus() + i);
	}
	else if (param.compare("devicenumber") == 0) {
		ud.setDeviceNumber(ud.getDeviceNumber() + i);
	}
	else if (param.compare("velo") == 0) {
		ud.setVelo(ud.getVeloRatio() + i);
	}
}
