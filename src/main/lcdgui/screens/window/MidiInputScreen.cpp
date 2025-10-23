#include "MidiInputScreen.hpp"
#include "lcdgui/screens/VmpcSettingsScreen.hpp"

using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;

MidiInputScreen::MidiInputScreen(mpc::Mpc& mpc, const int layerIndex)
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

    auto vmpcSettingsScreen = mpc.screens->get<VmpcSettingsScreen>();

    if (ls->isPreviousScreenNot<VmpcWarningSettingsIgnoredScreen>() &&
        vmpcSettingsScreen->midiControlMode == VmpcSettingsScreen::MidiControlMode::VMPC)
    {
        ls->Draw();
        mpc.getLayeredScreen()->openScreen<VmpcWarningSettingsIgnoredScreen>();
    }
}

void MidiInputScreen::function(int i)
{
	ScreenComponent::function(i);

	if (i == 1)
    {
        mpc.getLayeredScreen()->openScreen<MidiInputMonitorScreen>();
    }
}

void MidiInputScreen::turnWheel(int i)
{
	init();

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

    switch (type) {
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

void MidiInputScreen::setReceiveCh(int i)
{
	if (i < -1 || i > 15)
		return;
	
	receiveCh = i;
	displayReceiveCh();
}

int MidiInputScreen::getReceiveCh()
{
	return receiveCh;
}

void MidiInputScreen::setProgChangeSeq(bool b)
{
	if (progChangeSeq == b)
		return;

	progChangeSeq = b;
	displayProgChangeSeq();
}

bool MidiInputScreen::getProgChangeSeq()
{
	return progChangeSeq;
}

void MidiInputScreen::setSustainPedalToDuration(bool b)
{
	if (sustainPedalToDuration == b)
		return;

	sustainPedalToDuration = b;
	displaySustainPedalToDuration();
}

bool MidiInputScreen::isSustainPedalToDurationEnabled()
{
	return sustainPedalToDuration;
}

void MidiInputScreen::setMidiFilterEnabled(bool b)
{
	if (midiFilter == b)
		return;

	midiFilter = b;
	displayMidiFilter();
}

bool MidiInputScreen::isMidiFilterEnabled()
{
	return midiFilter;
}

void MidiInputScreen::setType(int i)
{
	if (i < 0 || i > 134)
    {
        return;
    }

	type = i;
	displayType();
    displayPass();
}

int MidiInputScreen::getType()
{
	return type;
}

void MidiInputScreen::setPass(bool b)
{
	switch (type) {
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

bool MidiInputScreen::isNotePassEnabled()
{
	return notePassEnabled;
}

void MidiInputScreen::setNotePassEnabled(bool b)
{
	notePassEnabled = b;
}

bool MidiInputScreen::isPitchBendPassEnabled()
{
	return pitchBendPassEnabled;
}

void MidiInputScreen::setPitchBendPassEnabled(bool b)
{
	pitchBendPassEnabled = b;
}

bool MidiInputScreen::isPgmChangePassEnabled()
{
	return pgmChangePassEnabled;
}

void MidiInputScreen::setPgmChangePassEnabled(bool b)
{
	pgmChangePassEnabled = b;
}

bool MidiInputScreen::isChPressurePassEnabled()
{
	return chPressurePassEnabled;
}

void MidiInputScreen::setChPressurePassEnabled(bool b)
{
	chPressurePassEnabled = b;
}

bool MidiInputScreen::isPolyPressurePassEnabled()
{
	return polyPressurePassEnabled;
}

void MidiInputScreen::setPolyPressurePassEnabled(bool b)
{
	polyPressurePassEnabled = b;
}

bool MidiInputScreen::isExclusivePassEnabled()
{
	return exclusivePassEnabled;
}

void MidiInputScreen::setExclusivePassEnabled(bool b)
{
	exclusivePassEnabled = b;
}

std::vector<bool>& MidiInputScreen::getCcPassEnabled()
{
    return ccPassEnabled;
}
