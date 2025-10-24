#include "VmpcAutoSaveScreen.hpp"
#include "VmpcSettingsScreen.hpp"

using namespace mpc::lcdgui::screens;

VmpcAutoSaveScreen::VmpcAutoSaveScreen(mpc::Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "vmpc-auto-save", layerIndex)
{
}

void VmpcAutoSaveScreen::open()
{
    findChild<TextComp>("msg")->setText("Only applies to standalone");

    displayAutoSaveOnExit();
    displayAutoLoadOnStart();

    auto vmpcSettingsScreen = mpc.screens->get<VmpcSettingsScreen>();
    auto midiControlMode = vmpcSettingsScreen->getMidiControlMode();
    ls->setFunctionKeysArrangement(midiControlMode == VmpcSettingsScreen::MidiControlMode::ORIGINAL ? 1 : 0);
}

void VmpcAutoSaveScreen::function(int i)
{
    switch (i)
    {
    case 0:
        mpc.getLayeredScreen()->openScreen<VmpcSettingsScreen>();
        break;
    case 1:
        mpc.getLayeredScreen()->openScreen<VmpcKeyboardScreen>();
        break;
    case 3:
        mpc.getLayeredScreen()->openScreen<VmpcDisksScreen>();
        break;
    case 4:
    {
        auto vmpcSettingsScreen = mpc.screens->get<VmpcSettingsScreen>();

        if (vmpcSettingsScreen->getMidiControlMode() == VmpcSettingsScreen::MidiControlMode::ORIGINAL)
        {
            return;
        }

        mpc.getLayeredScreen()->openScreen<VmpcMidiScreen>();
        break;
    }
    }
}

void VmpcAutoSaveScreen::turnWheel(int i)
{

    const auto focusedFieldName = getFocusedFieldName();

    if (focusedFieldName == "auto-save-on-exit")
    {
        setAutoSaveOnExit(autoSaveOnExit + i);
    }
    else if (focusedFieldName == "auto-load-on-start")
    {
        setAutoLoadOnStart(autoLoadOnStart + i);
    }
}

void VmpcAutoSaveScreen::setAutoSaveOnExit(int i)
{
    if (i < 0 || i > 1)
    {
        return;
    }

    autoSaveOnExit = i;

    displayAutoSaveOnExit();
}

void VmpcAutoSaveScreen::setAutoLoadOnStart(int i)
{
    if (i < 0 || i > 2)
    {
        return;
    }

    autoLoadOnStart = i;
    displayAutoLoadOnStart();
}

void VmpcAutoSaveScreen::displayAutoSaveOnExit()
{
    findField("auto-save-on-exit")->setText(autoSaveOnExitNames[autoSaveOnExit]);
}

void VmpcAutoSaveScreen::displayAutoLoadOnStart()
{
    findField("auto-load-on-start")->setText(autoLoadOnStartNames[autoLoadOnStart]);
}

int VmpcAutoSaveScreen::getAutoSaveOnExit()
{
    return autoSaveOnExit;
}

int VmpcAutoSaveScreen::getAutoLoadOnStart()
{
    return autoLoadOnStart;
}
