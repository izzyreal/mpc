#include "MidiOutputScreen.hpp"

#include "Mpc.hpp"
#include "StrUtil.hpp"
#include "audiomidi/MidiOutput.hpp"
#include "lcdgui/Label.hpp"
#include "lcdgui/LayeredScreen.hpp"
#include "lcdgui/screens/window/NameScreen.hpp"

#include "sequencer/Sequencer.hpp"
#include "sequencer/Sequence.hpp"
#include "sequencer/Track.hpp"

using namespace mpc::lcdgui::screens::window;

MidiOutputScreen::MidiOutputScreen(Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "midi-output", layerIndex)
{
}

void MidiOutputScreen::open()
{
    if (ls.lock()->isPreviousScreenNot(
            {ScreenId::NameScreen, ScreenId::MidiOutputMonitorScreen}))
    {
        const auto track = sequencer.lock()->getSelectedTrack();
        const auto dev = track->getDeviceIndex();

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

        const auto enterAction =
            [this, renameDeviceIndex](const std::string &nameScreenName)
        {
            sequencer.lock()->getSelectedSequence()->setDeviceName(renameDeviceIndex,
                                                            nameScreenName);
            openScreenById(ScreenId::MidiOutputScreen);
        };

        const auto nameScreen = mpc.screens->get<ScreenId::NameScreen>();
        const auto seq = sequencer.lock()->getSelectedSequence();
        nameScreen->initialize(seq->getDeviceName(renameDeviceIndex), 8,
                               enterAction, "midi-output");
        openScreenById(ScreenId::NameScreen);
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

void MidiOutputScreen::turnWheel(const int i)
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

void MidiOutputScreen::function(const int i)
{
    ScreenComponent::function(i);

    switch (i)
    {
        case 1:
            openScreenById(ScreenId::MidiOutputMonitorScreen);
            break;
        case 4:
            mpc.panic();
            break;
    }
}

void MidiOutputScreen::displaySoftThru() const
{
    findField("softthru")->setText(softThruNames[softThru]);
}

void MidiOutputScreen::displayDeviceName() const
{
    const auto sequence = sequencer.lock()->getSelectedSequence();
    const auto devName = sequence->getDeviceName(deviceIndex + 1);

    findField("firstletter")->setText(devName.substr(0, 1));
    findLabel("devicename")->setText(devName.substr(1, devName.length()));

    std::string devNumber;

    if (deviceIndex >= 16)
    {
        devNumber =
            StrUtil::padLeft(std::to_string(deviceIndex - 15), " ", 2) + "B";
    }
    else
    {
        devNumber =
            StrUtil::padLeft(std::to_string(deviceIndex + 1), " ", 2) + "A";
    }

    findField("devicenumber")->setText(devNumber);
}

void MidiOutputScreen::setSoftThru(const int i)
{
    softThru = std::clamp(i, 0, 4);
    displaySoftThru();
}

int MidiOutputScreen::getSoftThru() const
{
    return softThru;
}

void MidiOutputScreen::setDeviceIndex(const int i)
{
    deviceIndex = std::clamp(i, 0, 31);
    displayDeviceName();
}
