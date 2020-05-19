#include "UserScreen.hpp"

#include <lcdgui/Label.hpp>
#include <ui/UserDefaults.hpp>
#include <sequencer/TimeSignature.hpp>

using namespace mpc::lcdgui::screens;
using namespace std;

UserScreen::UserScreen(const int& layer) 
	: ScreenComponent("user", layer)
{
}

void UserScreen::open()
{
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

void UserScreen::function(int i)
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

void UserScreen::turnWheel(int i)
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


void UserScreen::displayTempo()
{
	auto tempo = ud.getTempo().toString();
	tempo = moduru::lang::StrUtil::padLeft(tempo, " ", 5);
	tempo = Util::replaceDotWithSmallSpaceDot(tempo);
	findField("tempo").lock()->setText(tempo);
}

void UserScreen::displayLoop()
{
	findField("loop").lock()->setText(ud.isLoopEnabled() ? "ON" : "OFF");
}

void UserScreen::displayTsig()
{
	findField("tsig").lock()->setText(to_string(timeSig.getNumerator()) + "/" + to_string(timeSig.getDenominator()));
}

void UserScreen::displayBars()
{
	findField("bars").lock()->setText(to_string(ud.getLastBarIndex() + 1));
}

void UserScreen::displayPgm()
{
	if (ud.getPgm() == 0)
	{
		findField("pgm").lock()->setText("OFF");
	}
	else
	{
		findField("pgm").lock()->setText(to_string(ud.getPgm()));
	}
}

void UserScreen::displayRecordingMode()
{
	findField("recordingmode").lock()->setText(ud.isRecordingModeMulti() ? "M" : "S");
}

void UserScreen::displayBus()
{
	findField("bus").lock()->setText(busNames[ud.getBus()]);
	displayDeviceName();
}

void UserScreen::displayDeviceNumber()
{
	if (ud.getDeviceNumber() == 0)
	{
		findField("devicenumber").lock()->setText("OFF");
	}
	else
	{
		if (ud.getDeviceNumber() >= 17)
		{
			findField("devicenumber").lock()->setTextPadded(to_string(ud.getDeviceNumber() - 16) + "B", " ");
		}
		else
		{
			findField("devicenumber").lock()->setTextPadded(to_string(ud.getDeviceNumber()) + "A", " ");
		}
	}
}

void UserScreen::displayVelo()
{
	findField("velo").lock()->setText(to_string(ud.getVeloRatio()));
}

void UserScreen::displayDeviceName()
{
	auto sampler = Mpc::instance().getSampler().lock();

	if (ud.getBus() != 0)
	{
		if (ud.getDeviceNumber() == 0)
		{
			auto p = dynamic_pointer_cast<mpc::sampler::Program>(sampler->getProgram(sampler->getDrumBusProgramNumber(ud.getBus())).lock());
			findLabel("devicename").lock()->setText(p->getName());
		}
		else
		{
			findLabel("devicename").lock()->setText(ud.getDeviceName(ud.getDeviceNumber()));
		}
	}

	if (ud.getBus() == 0)
	{
		if (ud.getDeviceNumber() != 0)
		{
			findLabel("devicename").lock()->setText(ud.getDeviceName(ud.getDeviceNumber()));
		}
		else
		{
			findLabel("devicename").lock()->setText("");
		}
	}
}
