#include "MidiOutputScreen.hpp"

#include <audiomidi/MidiOutput.hpp>
#include <lcdgui/screens/window/NameScreen.hpp>

#include <sequencer/Track.hpp>

using namespace mpc::lcdgui::screens::window;

MidiOutputScreen::MidiOutputScreen(mpc::Mpc& mpc, const int layerIndex)
	: ScreenComponent(mpc, "midi-output", layerIndex)
{
}

void MidiOutputScreen::open()
{
	init();

	if (ls->isPreviousScreenNot<NameScreen, MidiOutputMonitorScreen>())
	{
		auto dev = track->getDeviceIndex();

		if (dev > 0)
        {
			deviceIndex = dev - 1;
        }
		else
        {
			deviceIndex = 0;
        }
	}

	displaySoftThru();
	displayDeviceName();
}

void MidiOutputScreen::openNameScreen()
{
    if (param == "firstletter")
    {
        auto renameDeviceIndex = deviceIndex == 0 ? 1 : deviceIndex + 1;

        const auto enterAction = [this, renameDeviceIndex](std::string &nameScreenName) {
            sequencer.lock()->getActiveSequence()->setDeviceName(renameDeviceIndex, nameScreenName);
            mpc.getLayeredScreen()->openScreen<MidiOutputScreen>();
        };

        const auto nameScreen = mpc.screens->get<NameScreen>();
        auto seq = sequencer.lock()->getActiveSequence();
        nameScreen->initialize(seq->getDeviceName(renameDeviceIndex), 8, enterAction, "midi-output");
        mpc.getLayeredScreen()->openScreen<NameScreen>();
    }
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
		
	if (param == "softthru")
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
        mpc.getLayeredScreen()->openScreen<MidiOutputMonitorScreen>();
		break;
	case 4:
        mpc.panic();
		break;
	}
}

void MidiOutputScreen::displaySoftThru()
{
	findField("softthru")->setText(softThruNames[softThru]);
}

void MidiOutputScreen::displayDeviceName()
{
	auto sequence = sequencer.lock()->getActiveSequence();
	auto devName = sequence->getDeviceName(deviceIndex + 1);
	
	findField("firstletter")->setText(devName.substr(0, 1));
	findLabel("devicename")->setText(devName.substr(1, devName.length()));

	std::string devNumber;
	
	if (deviceIndex >= 16)
		devNumber = StrUtil::padLeft(std::to_string(deviceIndex - 15), " ", 2) + "B";
	else
		devNumber = StrUtil::padLeft(std::to_string(deviceIndex + 1), " ", 2) + "A";

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
