#include "MidiOutputScreen.hpp"

#include <lcdgui/screens/window/NameScreen.hpp>
#include <audiomidi/MpcMidiPorts.hpp>

#include <sequencer/Track.hpp>

using namespace mpc::lcdgui::screens::window;
using namespace std;

MidiOutputScreen::MidiOutputScreen(mpc::Mpc& mpc, const int layerIndex)
	: ScreenComponent(mpc, "midi-output", layerIndex)
{
}

void MidiOutputScreen::open()
{
	init();
	auto prevScreen = ls.lock()->getPreviousScreenName();
	if (prevScreen.compare("name") != 0 && prevScreen.compare("midi-output-monitor") != 0)
	{
		auto dev = track.lock()->getDevice();

		if (dev > 0)
			deviceIndex = dev - 1;
		else
			deviceIndex = 0;
	}

	displaySoftThru();
	displayDeviceName();
}

void MidiOutputScreen::turnWheel(int i)
{
	init();
	
	auto seq = sequencer.lock()->getActiveSequence().lock();
	
	if (param.compare("firstletter") == 0)
	{
		auto nameScreen = mpc.screens->get<NameScreen>("name");
		nameScreen->setName(seq->getDeviceName(deviceIndex + i));
		nameScreen->parameterName = "devicename";
		nameScreen->setNameLimit(8);
		openScreen("name");
	}
	else if (param.compare("softthru") == 0)
	{
		setSoftThru(softThru + i);
	}
	else if (param.compare("devicenumber") == 0)
	{
		setDeviceIndex(deviceIndex + i);
	}
}

void MidiOutputScreen::function(int i)
{
	ScreenComponent::function(i);
	
	switch (i)
	{
	case 1:
		openScreen("midi-output-monitor");
		break;
	case 4:
		mpc.getMidiPorts().lock()->panic();
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
	auto devName = sequence->getDeviceName(deviceIndex + 1);
	
	findField("firstletter").lock()->setText(devName.substr(0, 1));
	findLabel("devicename").lock()->setText(devName.substr(1, devName.length()));
	
	string devNumber = "";
	
	if (deviceIndex >= 16)
		devNumber = moduru::lang::StrUtil::padLeft(to_string(deviceIndex - 15), " ", 2) + "B";
	else
		devNumber = moduru::lang::StrUtil::padLeft(to_string(deviceIndex + 1), " ", 2) + "A";

	findField("devicenumber").lock()->setText(devNumber);
}

void MidiOutputScreen::setSoftThru(int i)
{
	if (i < 0 || i > 4)
		return;

	softThru = i;
	displaySoftThru();
}

int MidiOutputScreen::getSoftThru()
{
	return softThru;
}

void MidiOutputScreen::setDeviceIndex(int i)
{
	if (i < 0 || i > 31)
		return;

	deviceIndex = i;
	displayDeviceName();
}

int MidiOutputScreen::getDeviceNumber()
{
	return deviceIndex;
}

