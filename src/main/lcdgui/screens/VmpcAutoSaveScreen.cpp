#include "VmpcAutoSaveScreen.hpp"
#include "Mpc.hpp"
#include "VmpcSettingsScreen.hpp"
#include "lcdgui/LayeredScreen.hpp"

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

    const auto vmpcSettingsScreen =
        mpc.screens->get<ScreenId::VmpcSettingsScreen>();
    const auto midiControlMode = vmpcSettingsScreen->getMidiControlMode();
    ls->setFunctionKeysArrangement(
        midiControlMode == VmpcSettingsScreen::MidiControlMode::ORIGINAL ? 1
                                                                         : 0);
}

void VmpcAutoSaveScreen::function(int i)
{
    switch (i)
    {
        case 0:
            openScreenById(ScreenId::VmpcSettingsScreen);
            break;
        case 1:
            openScreenById(ScreenId::VmpcKeyboardScreen);
            break;
        case 3:
            openScreenById(ScreenId::VmpcDisksScreen);
            break;
        case 4:
        {
            const auto vmpcSettingsScreen =
                mpc.screens->get<ScreenId::VmpcSettingsScreen>();

            if (vmpcSettingsScreen->getMidiControlMode() ==
                VmpcSettingsScreen::MidiControlMode::ORIGINAL)
            {
                return;
            }

            openScreenById(ScreenId::VmpcMidiScreen);
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
    autoSaveOnExit = std::clamp(i, 0, 1);
    displayAutoSaveOnExit();
}

void VmpcAutoSaveScreen::setAutoLoadOnStart(int i)
{
    autoLoadOnStart = std::clamp(i, 0, 2);
    displayAutoLoadOnStart();
}

void VmpcAutoSaveScreen::displayAutoSaveOnExit()
{
    findField("auto-save-on-exit")
        ->setText(autoSaveOnExitNames[autoSaveOnExit]);
}

void VmpcAutoSaveScreen::displayAutoLoadOnStart()
{
    findField("auto-load-on-start")
        ->setText(autoLoadOnStartNames[autoLoadOnStart]);
}

int VmpcAutoSaveScreen::getAutoSaveOnExit()
{
    return autoSaveOnExit;
}

int VmpcAutoSaveScreen::getAutoLoadOnStart()
{
    return autoLoadOnStart;
}
