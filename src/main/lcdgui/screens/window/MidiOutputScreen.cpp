#include "MidiOutputScreen.hpp"

#include "audiomidi/MidiOutput.hpp"
#include "lcdgui/screens/window/NameScreen.hpp"

#include "sequencer/Track.hpp"

using namespace mpc::lcdgui::screens::window;

MidiOutputScreen::MidiOutputScreen(mpc::Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "midi-output", layerIndex)
{
}

void MidiOutputScreen::open()
{

    if (ls->isPreviousScreenNot<NameScreen, MidiOutputMonitorScreen>())
    {
        auto track = mpc.getSequencer()->getActiveTrack();
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
    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    if (focusedFieldName == "firstletter")
    {
        auto renameDeviceIndex = deviceIndex == 0 ? 1 : deviceIndex + 1;

        const auto enterAction = [this, renameDeviceIndex](std::string &nameScreenName)
        {
            sequencer->getActiveSequence()->setDeviceName(renameDeviceIndex, nameScreenName);
            mpc.getLayeredScreen()->openScreen<MidiOutputScreen>();
        };

        const auto nameScreen = mpc.screens->get<NameScreen>();
        auto seq = sequencer->getActiveSequence();
        nameScreen->initialize(seq->getDeviceName(renameDeviceIndex), 8, enterAction, "midi-output");
        mpc.getLayeredScreen()->openScreen<NameScreen>();
    }
}

void MidiOutputScreen::right()
{

    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    if (focusedFieldName == "firstletter")
    {
        openNameScreen();
    }
    else
    {
        ScreenComponent::right();
    }
}

void MidiOutputScreen::turnWheel(int i)
{

    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    if (focusedFieldName == "softthru")
    {
        setSoftThru(softThru + i);
    }
    else if (focusedFieldName == "devicenumber")
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
    auto sequence = sequencer->getActiveSequence();
    auto devName = sequence->getDeviceName(deviceIndex + 1);

    findField("firstletter")->setText(devName.substr(0, 1));
    findLabel("devicename")->setText(devName.substr(1, devName.length()));

    std::string devNumber;

    if (deviceIndex >= 16)
    {
        devNumber = StrUtil::padLeft(std::to_string(deviceIndex - 15), " ", 2) + "B";
    }
    else
    {
        devNumber = StrUtil::padLeft(std::to_string(deviceIndex + 1), " ", 2) + "A";
    }

    findField("devicenumber")->setText(devNumber);
}

void MidiOutputScreen::setSoftThru(int i)
{
    softThru = std::clamp(i, 0, 4);
    displaySoftThru();
}

int MidiOutputScreen::getSoftThru()
{
    return softThru;
}

void MidiOutputScreen::setDeviceIndex(int i)
{
    deviceIndex = std::clamp(i, 0, 31);
    displayDeviceName();
}
