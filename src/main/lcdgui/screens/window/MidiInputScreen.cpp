#include "MidiInputScreen.hpp"
#include "lcdgui/screens/VmpcSettingsScreen.hpp"

using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;

MidiInputScreen::MidiInputScreen(Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "midi-input", layerIndex)
{
}

void MidiInputScreen::open()
{
    displayReceiveCh();
    displayProgChangeSeq();
    displaySustainPedalToDuration();
    displayMidiFilter();
    displayType();
    displayPass();
}

void MidiInputScreen::function(const int i)
{
    ScreenComponent::function(i);

    if (i == 1)
    {
        openScreenById(ScreenId::MidiInputMonitorScreen);
    }
}

void MidiInputScreen::turnWheel(const int i)
{

    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    if (focusedFieldName == "receivech")
    {
        setReceiveCh(receiveCh + i);
    }
    else if (focusedFieldName == "seq")
    {
        setProgChangeSeq(i > 0);
    }
    else if (focusedFieldName == "duration")
    {
        setSustainPedalToDuration(i > 0);
    }
    else if (focusedFieldName == "midifilter")
    {
        setMidiFilterEnabled(i > 0);
    }
    else if (focusedFieldName == "type")
    {
        setType(type + i);
    }
    else if (focusedFieldName == "pass")
    {
        setPass(i > 0);
    }
}

void MidiInputScreen::displayPass()
{
    bool pass;

    switch (type)
    {
        case 0:
            pass = notePassEnabled;
            break;
        case 1:
            pass = pitchBendPassEnabled;
            break;
        case 2:
            pass = pgmChangePassEnabled;
            break;
        case 3:
            pass = chPressurePassEnabled;
            break;
        case 4:
            pass = polyPressurePassEnabled;
            break;
        case 5:
            pass = exclusivePassEnabled;
            break;
        default:
            const uint8_t ccIndex = type - 6;
            pass = ccPassEnabled[ccIndex];
            break;
    }

    findField("pass")->setText(pass ? "YES" : "NO");
}

void MidiInputScreen::displayType()
{
    findField("type")->setText(typeNames[type]);
}

void MidiInputScreen::displayMidiFilter()
{
    findField("midifilter")->setText(midiFilter ? "ON" : "OFF");
}

void MidiInputScreen::displaySustainPedalToDuration()
{
    findField("duration")->setText(sustainPedalToDuration ? "ON" : "OFF");
}

void MidiInputScreen::displayProgChangeSeq()
{
    findField("seq")->setText(progChangeSeq ? "ON" : "OFF");
}

void MidiInputScreen::displayReceiveCh()
{
    if (receiveCh == -1)
    {
        findField("receivech")->setText("ALL");
    }
    else
    {
        findField("receivech")->setText(std::to_string(receiveCh + 1));
    }
}

void MidiInputScreen::setReceiveCh(const int i)
{
    receiveCh = std::clamp(i, -1, 15);
    displayReceiveCh();
}

int MidiInputScreen::getReceiveCh() const
{
    return receiveCh;
}

void MidiInputScreen::setProgChangeSeq(const bool b)
{
    if (progChangeSeq == b)
    {
        return;
    }

    progChangeSeq = b;
    displayProgChangeSeq();
}

bool MidiInputScreen::getProgChangeSeq() const
{
    return progChangeSeq;
}

void MidiInputScreen::setSustainPedalToDuration(const bool b)
{
    if (sustainPedalToDuration == b)
    {
        return;
    }

    sustainPedalToDuration = b;
    displaySustainPedalToDuration();
}

bool MidiInputScreen::isSustainPedalToDurationEnabled() const
{
    return sustainPedalToDuration;
}

void MidiInputScreen::setMidiFilterEnabled(const bool b)
{
    if (midiFilter == b)
    {
        return;
    }

    midiFilter = b;
    displayMidiFilter();
}

bool MidiInputScreen::isMidiFilterEnabled() const
{
    return midiFilter;
}

void MidiInputScreen::setType(const int i)
{
    type = std::clamp(i, 0, 134);
    displayType();
    displayPass();
}

int MidiInputScreen::getType() const
{
    return type;
}

void MidiInputScreen::setPass(const bool b)
{
    switch (type)
    {
        case 0:
            notePassEnabled = b;
            break;
        case 1:
            pitchBendPassEnabled = b;
            break;
        case 2:
            pgmChangePassEnabled = b;
            break;
        case 3:
            chPressurePassEnabled = b;
            break;
        case 4:
            polyPressurePassEnabled = b;
            break;
        case 5:
            exclusivePassEnabled = b;
            break;
        default:
            const uint8_t ccIndex = type - 6;
            ccPassEnabled[ccIndex] = b;
            break;
    }

    displayPass();
}

bool MidiInputScreen::isNotePassEnabled() const
{
    return notePassEnabled;
}

void MidiInputScreen::setNotePassEnabled(const bool b)
{
    notePassEnabled = b;
}

bool MidiInputScreen::isPitchBendPassEnabled() const
{
    return pitchBendPassEnabled;
}

void MidiInputScreen::setPitchBendPassEnabled(const bool b)
{
    pitchBendPassEnabled = b;
}

bool MidiInputScreen::isPgmChangePassEnabled() const
{
    return pgmChangePassEnabled;
}

void MidiInputScreen::setPgmChangePassEnabled(const bool b)
{
    pgmChangePassEnabled = b;
}

bool MidiInputScreen::isChPressurePassEnabled() const
{
    return chPressurePassEnabled;
}

void MidiInputScreen::setChPressurePassEnabled(const bool b)
{
    chPressurePassEnabled = b;
}

bool MidiInputScreen::isPolyPressurePassEnabled() const
{
    return polyPressurePassEnabled;
}

void MidiInputScreen::setPolyPressurePassEnabled(const bool b)
{
    polyPressurePassEnabled = b;
}

bool MidiInputScreen::isExclusivePassEnabled() const
{
    return exclusivePassEnabled;
}

void MidiInputScreen::setExclusivePassEnabled(const bool b)
{
    exclusivePassEnabled = b;
}

std::vector<bool> &MidiInputScreen::getCcPassEnabled()
{
    return ccPassEnabled;
}
