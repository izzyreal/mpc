#include "VmpcSettingsScreen.hpp"

#include "lcdgui/screens/dialog2/PopupScreen.hpp"

using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::dialog2;

VmpcSettingsScreen::VmpcSettingsScreen(mpc::Mpc& mpc, const int layerIndex)
	: ScreenComponent(mpc, "vmpc-settings", layerIndex)
{
}

void VmpcSettingsScreen::open()
{
	displayInitialPadMapping();
    display16LevelsEraseMode();
    displayAutoConvertWavs();
    displayMidiControlMode();
    ls->setFunctionKeysArrangement(midiControlMode == MidiControlMode::ORIGINAL ? 1 : 0);
}

void VmpcSettingsScreen::function(int i)
{
    switch (i)
    {
        case 1:
            openScreen("vmpc-keyboard");
            break;
        case 2:
            openScreen("vmpc-auto-save");
            break;
        case 3:
            openScreen("vmpc-disks");
            break;
        case 4:
            if (midiControlMode == MidiControlMode::ORIGINAL)
            {
                return;
            }

            openScreen("vmpc-midi");
            break;
    }
}

void VmpcSettingsScreen::turnWheel(int i)
{
    init();

	if (param == "initial-pad-mapping")
	{
		setInitialPadMapping(initialPadMapping + i);
	}
    else if (param == "16-levels-erase-mode")
    {
        set16LevelsEraseMode(_16LevelsEraseMode + i);
    }
    else if (param == "auto-convert-wavs")
    {
        setAutoConvertWavs(autoConvertWavs + i);
    }
    else if (param == "midi-control-mode")
    {
        setMidiControlMode(midiControlMode + i);
        ls->setFunctionKeysArrangement(midiControlMode == MidiControlMode::ORIGINAL ? 1 : 0);
    }
}

void VmpcSettingsScreen::setInitialPadMapping(int i)
{
	if (i < 0 || i > 1)
		return;

	initialPadMapping = i;

	displayInitialPadMapping();
}

void VmpcSettingsScreen::displayInitialPadMapping()
{
	findField("initial-pad-mapping")->setText(initialPadMappingNames[initialPadMapping]);
}

void VmpcSettingsScreen::display16LevelsEraseMode()
{
    findField("16-levels-erase-mode")->setText(_16LevelsEraseModeNames[_16LevelsEraseMode]);
}

void VmpcSettingsScreen::set16LevelsEraseMode(int i)
{
    if (i < 0 || i > 1)
        return;

    _16LevelsEraseMode = i;
    display16LevelsEraseMode();
}

void VmpcSettingsScreen::setAutoConvertWavs(int i)
{
    if (i < 0 || i > 1)
        return;

    autoConvertWavs = i;
    displayAutoConvertWavs();
}

void VmpcSettingsScreen::displayAutoConvertWavs()
{
    findField("auto-convert-wavs")->setText(autoConvertWavs == 1 ? "YES" : "ASK");
}

void VmpcSettingsScreen::setMidiControlMode(int i)
{
    if (i < 0 || i > 1)
        return;

    midiControlMode = i;
    displayMidiControlMode();
}

void VmpcSettingsScreen::displayMidiControlMode()
{
    findField("midi-control-mode")->setText(midiControlModeNames[midiControlMode]);
}

int VmpcSettingsScreen::getMidiControlMode()
{
    return midiControlMode;
}
