#include "MidiOutputScreen.hpp"

#include <lcdgui/screens/window/NameScreen.hpp>
#include <audiomidi/MpcMidiOutput.hpp>

#include <sequencer/Track.hpp>

using namespace mpc::lcdgui::screens::window;

MidiOutputScreen::MidiOutputScreen(mpc::Mpc& mpc, const int layerIndex)
	: ScreenComponent(mpc, "midi-output", layerIndex)
{
}

void MidiOutputScreen::open()
{
	init();
	auto prevScreen = ls->getPreviousScreenName();
	if (prevScreen != "name" && prevScreen != "midi-output-monitor")
	{
		auto dev = track->getDeviceIndex();

		if (dev > 0)
			deviceIndex = dev - 1;
		else
			deviceIndex = 0;
	}

	displaySoftThru();
	displayDeviceName();
}

void MidiOutputScreen::openNameScreen()
{
    auto renameDeviceIndex = deviceIndex == 0 ? 1 : deviceIndex + 1;

    const auto enterAction = [this, renameDeviceIndex](std::string& nameScreenName) {
        sequencer->getActiveSequence()->setDeviceName(renameDeviceIndex, nameScreenName);
        openScreen(name);
    };

    const auto nameScreen = mpc.screens->get<NameScreen>("name");
    auto seq = sequencer->getActiveSequence();
    nameScreen->initialize(seq->getDeviceName(renameDeviceIndex), 8, enterAction, name);
    openScreen("name");
}

void MidiOutputScreen::right()
{
    init();
    if (param == "firstletter")
        openNameScreen();
    else
        ScreenComponent::right();
}

void MidiOutputScreen::turnWheel(int i)
{
	init();
		
	if (param == "firstletter")
	{
        openNameScreen();
	}
	else if (param == "softthru")
	{
		setSoftThru(softThru + i);
	}
	else if (param == "devicenumber")
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
        mpc.getMidiOutput()->panic();
		break;
	}
}

void MidiOutputScreen::displaySoftThru()
{
	findField("softthru")->setText(softThruNames[softThru]);
}

void MidiOutputScreen::displayDeviceName()
{
	auto sequence = sequencer->getActiveSequence();
	auto devName = sequence->getDeviceName(deviceIndex + 1);
	
	findField("firstletter")->setText(devName.substr(0, 1));
	findLabel("devicename")->setText(devName.substr(1, devName.length()));

	std::string devNumber;
	
	if (deviceIndex >= 16)
		devNumber = moduru::lang::StrUtil::padLeft(std::to_string(deviceIndex - 15), " ", 2) + "B";
	else
		devNumber = moduru::lang::StrUtil::padLeft(std::to_string(deviceIndex + 1), " ", 2) + "A";

	findField("devicenumber")->setText(devNumber);
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
