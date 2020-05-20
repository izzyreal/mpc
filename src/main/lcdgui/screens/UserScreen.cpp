#include "UserScreen.hpp"

#include <lcdgui/Label.hpp>
#include <ui/UserDefaults.hpp>
#include <sequencer/TimeSignature.hpp>

#include <lang/StrUtil.hpp>

#include <Util.hpp>

using namespace mpc::lcdgui::screens;
using namespace moduru::lang;
using namespace std;

UserScreen::UserScreen(const int& layer) 
	: ScreenComponent("user", layer)
{
}

void UserScreen::open()
{
	mpc::ui::UserDefaults::instance().addObserver(this);
	mpc::ui::UserDefaults::instance().getTimeSig().addObserver(this);
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

void UserScreen::close()
{
	mpc::ui::UserDefaults::instance().deleteObserver(this);
	mpc::ui::UserDefaults::instance().getTimeSig().deleteObserver(this);
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
	auto tempo = mpc::ui::UserDefaults::instance().getTempo().toString();
	tempo = StrUtil::padLeft(tempo, " ", 5);
	tempo = mpc::Util::replaceDotWithSmallSpaceDot(tempo);
	findField("tempo").lock()->setText(tempo);
}

void UserScreen::displayLoop()
{
	findField("loop").lock()->setText(mpc::ui::UserDefaults::instance().isLoopEnabled() ? "ON" : "OFF");
}

void UserScreen::displayTsig()
{
	auto numerator = to_string(mpc::ui::UserDefaults::instance().getTimeSig().getNumerator());
	auto denominator = to_string(mpc::ui::UserDefaults::instance().getTimeSig().getDenominator());
	findField("tsig").lock()->setText(numerator + "/" + denominator);
}

void UserScreen::displayBars()
{
	findField("bars").lock()->setText(to_string(mpc::ui::UserDefaults::instance().getLastBarIndex() + 1));
}

void UserScreen::displayPgm()
{
	if (mpc::ui::UserDefaults::instance().getPgm() == 0)
	{
		findField("pgm").lock()->setText("OFF");
	}
	else
	{
		findField("pgm").lock()->setText(to_string(mpc::ui::UserDefaults::instance().getPgm()));
	}
}

void UserScreen::displayRecordingMode()
{
	findField("recordingmode").lock()->setText(mpc::ui::UserDefaults::instance().isRecordingModeMulti() ? "M" : "S");
}

void UserScreen::displayBus()
{
	findField("bus").lock()->setText(busNames[mpc::ui::UserDefaults::instance().getBus()]);
	displayDeviceName();
}

void UserScreen::displayDeviceNumber()
{
	if (mpc::ui::UserDefaults::instance().getDeviceNumber() == 0)
	{
		findField("devicenumber").lock()->setText("OFF");
	}
	else
	{
		if (mpc::ui::UserDefaults::instance().getDeviceNumber() >= 17)
		{
			findField("devicenumber").lock()->setTextPadded(to_string(mpc::ui::UserDefaults::instance().getDeviceNumber() - 16) + "B", " ");
		}
		else
		{
			findField("devicenumber").lock()->setTextPadded(to_string(mpc::ui::UserDefaults::instance().getDeviceNumber()) + "A", " ");
		}
	}
}

void UserScreen::displayVelo()
{
	findField("velo").lock()->setText(to_string(mpc::ui::UserDefaults::instance().getVeloRatio()));
}

void UserScreen::displayDeviceName()
{
	auto sampler = Mpc::instance().getSampler().lock();

	if (mpc::ui::UserDefaults::instance().getBus() != 0)
	{
		if (mpc::ui::UserDefaults::instance().getDeviceNumber() == 0)
		{
			auto p = dynamic_pointer_cast<mpc::sampler::Program>(sampler->getProgram(sampler->getDrumBusProgramNumber(mpc::ui::UserDefaults::instance().getBus())).lock());
			findLabel("devicename").lock()->setText(p->getName());
		}
		else
		{
			findLabel("devicename").lock()->setText(mpc::ui::UserDefaults::instance().getDeviceName(mpc::ui::UserDefaults::instance().getDeviceNumber()));
		}
	}

	if (mpc::ui::UserDefaults::instance().getBus() == 0)
	{
		if (mpc::ui::UserDefaults::instance().getDeviceNumber() != 0)
		{
			findLabel("devicename").lock()->setText(mpc::ui::UserDefaults::instance().getDeviceName(mpc::ui::UserDefaults::instance().getDeviceNumber()));
		}
		else
		{
			findLabel("devicename").lock()->setText("");
		}
	}
}

void UserScreen::update(moduru::observer::Observable* o, nonstd::any arg)
{
	string s = nonstd::any_cast<string>(arg);
	if (s.compare("tempo") == 0)
	{
		displayTempo();
	}
	else if (s.compare("loop") == 0)
	{
		displayLoop();
	}
	else if (s.compare("timesignature") == 0)
	{
		displayTsig();
	}
	else if (s.compare("bars") == 0)
	{
		displayBars();
	}
	else if (s.compare("pgm") == 0)
	{
		displayPgm();
	}
	else if (s.compare("recordingmode") == 0)
	{
		displayRecordingMode();
	}
	else if (s.compare("tracktype") == 0)
	{
		displayBus();
	}
	else if (s.compare("devicenumber") == 0)
	{
		displayDeviceNumber();
		displayDeviceName();
	}
	else if (s.compare("velo") == 0)
	{
		displayVelo();
	}
}
