#include "VmpcAutoSaveScreen.hpp"
#include "Mpc.hpp"
#include "VmpcSettingsScreen.hpp"
#include "lcdgui/LayeredScreen.hpp"

using namespace mpc::lcdgui::screens;

VmpcAutoSaveScreen::VmpcAutoSaveScreen(Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "vmpc-auto-save", layerIndex)
{
}

void VmpcAutoSaveScreen::open()
{
    findChild<TextComp>("msg")->setText("Only applies to standalone");

    displayAutoSaveOnExit();
    displayAutoLoadOnStart();
}

void VmpcAutoSaveScreen::function(const int i)
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
            openScreenById(ScreenId::VmpcMidiScreen);
            break;
        }
        default:;
    }
}

void VmpcAutoSaveScreen::turnWheel(const int i)
{
    if (const auto focusedFieldName = getFocusedFieldName();
        focusedFieldName == "auto-save-on-exit")
    {
        setAutoSaveOnExit(autoSaveOnExit + i);
    }
    else if (focusedFieldName == "auto-load-on-start")
    {
        setAutoLoadOnStart(autoLoadOnStart + i);
    }
}

void VmpcAutoSaveScreen::setAutoSaveOnExit(const int i)
{
    autoSaveOnExit = std::clamp(i, 0, 1);
    displayAutoSaveOnExit();
}

void VmpcAutoSaveScreen::setAutoLoadOnStart(const int i)
{
    autoLoadOnStart = std::clamp(i, 0, 2);
    displayAutoLoadOnStart();
}

void VmpcAutoSaveScreen::displayAutoSaveOnExit() const
{
    findField("auto-save-on-exit")
        ->setText(autoSaveOnExitNames[autoSaveOnExit]);
}

void VmpcAutoSaveScreen::displayAutoLoadOnStart() const
{
    findField("auto-load-on-start")
        ->setText(autoLoadOnStartNames[autoLoadOnStart]);
}

int VmpcAutoSaveScreen::getAutoSaveOnExit() const
{
    return autoSaveOnExit;
}

int VmpcAutoSaveScreen::getAutoLoadOnStart() const
{
    return autoLoadOnStart;
}
