#include "MidiOutputScreen.hpp"

#include <lcdgui/Label.hpp>
#include <ui/NameGui.hpp>
#include <sequencer/Sequence.hpp>

#include <lang/StrUtil.hpp>

using namespace mpc::lcdgui::screens::window;
using namespace std;

MidiOutputScreen::MidiOutputScreen(const int& layer)
	: ScreenComponent("midioutput", layer)
{
}

void MidiOutputScreen::open()
{
	displaySoftThru();
	displayDeviceName();
}

void MidiOutputScreen::turnWheel(int i)
{
	init();
	
	auto seq = sequencer.lock()->getActiveSequence().lock();
	
	if (param.compare("firstletter") == 0)
	{
		nameGui->setName(seq->getDeviceName(deviceNumber + i));
		nameGui->setParameterName("devicename");
		nameGui->setNameLimit(8);
		ls.lock()->openScreen("name");
	}
	else if (param.compare("softthru") == 0)
	{
		setSoftThru(softThru + i);
	}
	else if (param.compare("devicenumber") == 0)
	{
		setDeviceNumber(deviceNumber + i);
	}
}

void MidiOutputScreen::function(int i)
{
	BaseControls::function(i);
	
	switch (i)
	{
	case 1:
		ls.lock()->openScreen("midioutputmonitor");
		break;
	}
}

void MidiOutputScreen::displaySoftThru()
{
	findField("softthru").lock()->setText(softThruNames[softThru]);
}

void MidiOutputScreen::displayDeviceName()
{
	auto sequence = sequencer.lock()->getActiveSequence().lock();
	auto devName = sequence->getDeviceName(deviceNumber + 1);
	
	findField("firstletter").lock()->setText(devName.substr(0, 1));
	findLabel("devicename").lock()->setText(devName.substr(1, devName.length()));
	
	string devNumber = "";
	
	if (deviceNumber >= 16)
	{
		devNumber = moduru::lang::StrUtil::padLeft(to_string(deviceNumber - 15), " ", 2) + "B";
	}
	else {
		devNumber = moduru::lang::StrUtil::padLeft(to_string(deviceNumber + 1), " ", 2) + "A";
	}
	findField("devicenumber").lock()->setText(devNumber);
}

void MidiOutputScreen::setSoftThru(int i)
{
	if (i < 0 || i > 4)
	{
		return;
	}

	softThru = i;
	displaySoftThru();
}

int MidiOutputScreen::getSoftThru()
{
	return softThru;
}

void MidiOutputScreen::setDeviceNumber(int i)
{
	if (i < 0 || i > 31)
	{
		return;
	}

	deviceNumber = i;
	displayDeviceName();
}

int MidiOutputScreen::getDeviceNumber()
{
	return deviceNumber;
}

