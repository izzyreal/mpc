#include "MidiInputScreen.hpp"
#include "lcdgui/screens/VmpcSettingsScreen.hpp"

using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;
using namespace std;

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

    auto vmpcSettingsScreen = mpc.screens->get<VmpcSettingsScreen>("vmpc-settings");

    if (ls.lock()->getPreviousScreenName() != "vmpc-warning-settings-ignored" &&
        vmpcSettingsScreen->midiControlMode == VmpcSettingsScreen::MidiControlMode::VMPC)
    {
        ls.lock()->Draw();
        openScreen("vmpc-warning-settings-ignored");
    }
}

void MidiInputScreen::function(int i)
{
	ScreenComponent::function(i);

	if (i == 1)
    {
        openScreen("midi-input-monitor");
    }
}

void MidiInputScreen::turnWheel(int i)
{
	init();

	if (param == "receivech")
	{
		setReceiveCh(receiveCh + i);
	}
	else if (param == "seq")
	{
		setProgChangeSeq(i > 0);
	}
	else if (param == "duration")
	{
		setSustainPedalToDuration(i > 0);
	}
	else if (param == "midifilter")
	{
		setMidiFilterEnabled(i > 0);
	}
	else if (param == "type")
	{
		setType(type + i);
	}
	else if (param == "pass")
	{
		setPass(i > 0);
	}
}

void MidiInputScreen::displayPass()
{
	findField("pass").lock()->setText(pass ? "YES" : "NO");
}

void MidiInputScreen::displayType()
{
	findField("type").lock()->setText(typeNames[type]);
}

void MidiInputScreen::displayMidiFilter()
{
	findField("midifilter").lock()->setText(midiFilter ? "ON" : "OFF");
}

void MidiInputScreen::displaySustainPedalToDuration()
{
	findField("duration").lock()->setText(sustainPedalToDuration ? "ON" : "OFF");
}

void MidiInputScreen::displayProgChangeSeq()
{
	findField("seq").lock()->setText(progChangeSeq ? "ON" : "OFF");
}

void MidiInputScreen::displayReceiveCh()
{
	if (receiveCh == -1)
	{
		findField("receivech").lock()->setText("ALL");
	}
	else
	{
		findField("receivech").lock()->setText(to_string(receiveCh + 1));
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
		return;

	type = i;
	displayType();
}

int MidiInputScreen::getType()
{
	return type;
}

void MidiInputScreen::setPass(bool b)
{
	if (pass == b)
		return;

	pass = b;
	displayPass();
}

bool MidiInputScreen::getPass()
{
	return pass;
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
