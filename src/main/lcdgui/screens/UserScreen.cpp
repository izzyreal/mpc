#include "UserScreen.hpp"

#include "Mpc.hpp"
#include "lcdgui/screens/EventsScreen.hpp"

#include "Util.hpp"

#include "StrUtil.hpp"
#include "lcdgui/Label.hpp"
#include "sampler/Sampler.hpp"
#include "sequencer/Bus.hpp"
#include "sequencer/Sequencer.hpp"

using namespace mpc::lcdgui::screens;
using namespace mpc::sequencer;

UserScreen::UserScreen(Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "user", layerIndex)
{
    resetPreferences();
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

void UserScreen::function(const int i)
{
    switch (i)
    {
        // Intentional fall-through
        case 0:
        case 1:
        case 2:
        {
            const auto eventsScreen =
                mpc.screens->get<ScreenId::EventsScreen>();
            eventsScreen->tab = i;
            ls->openScreen(eventsScreen->tabNames[eventsScreen->tab]);
            break;
        }
        default:;
    }
}

void UserScreen::turnWheel(const int increment)
{
    if (const auto focusedFieldName = getFocusedFieldName();
        focusedFieldName == "tempo")
    {
        const double newTempo = tempo + increment * 0.1;
        setTempo(newTempo);
    }
    else if (focusedFieldName == "loop")
    {
        setLoop(increment > 0);
    }
    else if (focusedFieldName == "tsig")
    {
        if (increment > 0)
        {
            timeSig.increase();
        }
        else
        {
            timeSig.decrease();
        }

        displayTsig();
    }
    else if (focusedFieldName == "bars")
    {
        setLastBar(lastBar + increment);
    }
    else if (focusedFieldName == "pgm")
    {
        setPgm(pgm + increment);
    }
    else if (focusedFieldName == "recordingmode")
    {
        setRecordingModeMulti(increment > 0);
    }
    else if (focusedFieldName == "bus")
    {
        setBus(busType + increment);
    }
    else if (focusedFieldName == "device")
    {
        setDeviceNumber(device + increment);
    }
    else if (focusedFieldName == "velo")
    {
        setVelo(velo + increment);
    }
}
int UserScreen::getLastBar() const
{
    return lastBar;
}

void UserScreen::displayTempo() const
{
    findField("tempo")->setText(Util::tempoString(tempo));
}

void UserScreen::displayLoop() const
{
    findField("loop")->setText(loop ? "ON" : "OFF");
}

void UserScreen::displayTsig() const
{
    const auto numerator = std::to_string(timeSig.getNumerator());
    const auto denominator = std::to_string(timeSig.getDenominator());
    findField("tsig")->setText(numerator + "/" + denominator);
}

void UserScreen::displayBars() const
{
    findField("bars")->setText(std::to_string(lastBar + 1));
}

void UserScreen::displayPgm() const
{
    if (pgm == 0)
    {
        findField("pgm")->setText("OFF");
    }
    else
    {
        findField("pgm")->setText(std::to_string(pgm));
    }
}

void UserScreen::displayRecordingMode() const
{
    findField("recordingmode")->setText(recordingModeMulti ? "M" : "S");
}

void UserScreen::displayBus()
{
    findField("bus")->setText(busTypeToString(busType));
    displayDeviceName();
}

void UserScreen::displayDeviceNumber() const
{
    if (device == 0)
    {
        findField("device")->setText("OFF");
        return;
    }

    if (device >= 17)
    {
        findField("device")->setTextPadded(std::to_string(device - 16) + "B",
                                           " ");
    }
    else
    {
        findField("device")->setTextPadded(std::to_string(device) + "A", " ");
    }
}

void UserScreen::displayVelo() const
{
    findField("velo")->setText(std::to_string(velo));
}

void UserScreen::displayDeviceName()
{
    if (busType == BusType::MIDI)
    {
        if (device != 0)
        {
            findLabel("devicename")->setText(getDeviceName(device));
        }
        else
        {
            findLabel("devicename")->setText("");
        }
    }
    else
    {
        if (device == 0)
        {
            const auto programName =
                sampler
                    ->getProgram(sequencer->getDrumBus(busType)->getProgramIndex())
                    ->getName();
            findLabel("devicename")->setText(programName);
        }
        else
        {
            findLabel("devicename")->setText(getDeviceName(device));
        }
    }
}

void UserScreen::resetPreferences()
{
    sequenceName = std::string("Sequence");
    busType = BusType::DRUM1;
    tempo = 120.0;
    velo = 100;
    pgm = 0;
    recordingModeMulti = false;
    loop = true;
    device = 0;

    trackNames.clear();

    for (int i = 0; i < 64; i++)
    {
        trackNames.push_back(std::string(
            "Track-" + StrUtil::padLeft(std::to_string(i + 1), "0", 2)));
    }

    lastBar = 1;
    timeSig.setNumerator(4);
    timeSig.setDenominator(4);

    deviceNames.clear();

    deviceNames.push_back(std::string("        "));

    for (int i = 1; i < 33; i++)
    {
        deviceNames.push_back("Device" +
                              StrUtil::padLeft(std::to_string(i), "0", 2));
    }
}

std::string UserScreen::getDeviceName(const int i)
{
    return deviceNames[i];
}

void UserScreen::setTempo(const double newTempo)
{
    auto str = std::to_string(newTempo);

    if (newTempo < 30.0)
    {
        tempo = 30.0;
    }
    else if (newTempo > 300.0)
    {
        tempo = 300.0;
    }
    else
    {
        tempo = newTempo;
    }

    displayTempo();
}

void UserScreen::setLoop(const bool b)
{
    loop = b;
    displayLoop();
}

void UserScreen::setBus(const BusType busTypeToUse)
{
    busType = busTypeToUse;
    displayBus();
    displayDeviceName();
}

void UserScreen::setDeviceNumber(const int i)
{
    device = std::clamp(i, 0, 33);
    displayDeviceNumber();
    displayDeviceName();
}

void UserScreen::setRecordingModeMulti(const bool b)
{
    recordingModeMulti = b;
    displayRecordingMode();
}

void UserScreen::setLastBar(const int i)
{
    lastBar =
        std::clamp(i, 0, static_cast<int>(Mpc2000XlSpecs::MAX_LAST_BAR_INDEX));
    displayBars();
}

void UserScreen::setPgm(const int i)
{
    pgm = std::clamp(i, 0, 128);
    displayPgm();
}

void UserScreen::setVelo(const int i)
{
    velo = std::clamp(i, 1, 200);
    displayVelo();
}

std::string UserScreen::getTrackName(const int i)
{
    return trackNames[i];
}

int8_t UserScreen::getTrackStatus() const
{
    // This number is so magic that I forgot what it's for.
    return 6;
}

void UserScreen::setDeviceName(const int i, const std::string &s)
{
    deviceNames[i] = s;
}

void UserScreen::setSequenceName(const std::string &name)
{
    sequenceName = name;
}

void UserScreen::setTimeSig(const int num, const int den)
{
    timeSig.setNumerator(num);
    timeSig.setDenominator(den);
}

void UserScreen::setTrackName(const int i, const std::string &s)
{
    trackNames[i] = s;
}

double UserScreen::getTempo() const
{
    return tempo;
}
