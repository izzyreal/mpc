#include "VmpcSettingsScreen.hpp"
#include "lcdgui/LayeredScreen.hpp"

using namespace mpc::lcdgui::screens;

VmpcSettingsScreen::VmpcSettingsScreen(Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "vmpc-settings", layerIndex)
{
    easterEgg = std::make_shared<Background>();
    easterEgg->Hide(true);
    easterEgg->setBackgroundName("jd");
    addChild(easterEgg);
}

void VmpcSettingsScreen::open()
{
    displayInitialPadMapping();
    display16LevelsEraseMode();
    displayAutoConvertWavs();
    displayNameTypingWithKeyboard();
}

void VmpcSettingsScreen::close()
{
    if (!easterEgg->IsHidden())
    {
        easterEgg->setScrolling(false);
        easterEgg->Hide(true);
    }
}

void VmpcSettingsScreen::function(const int i)
{
    switch (i)
    {
        case 1:
            openScreenById(ScreenId::VmpcKeyboardScreen);
            break;
        case 2:
            openScreenById(ScreenId::VmpcAutoSaveScreen);
            break;
        case 3:
            openScreenById(ScreenId::VmpcDisksScreen);
            break;
        case 4:
            openScreenById(ScreenId::VmpcMidiScreen);
            break;
        case 5:
            if (easterEgg->IsHidden())
            {
                easterEgg->Hide(false);
                bringToFront(easterEgg.get());
                easterEgg->setScrolling(true);
            }
            else
            {
                easterEgg->setScrolling(false);
                easterEgg->Hide(true);
                SetDirty();
            }
            break;
        default:;
    }
}

void VmpcSettingsScreen::turnWheel(const int i)
{
    if (const auto focusedFieldName = getFocusedFieldName();
        focusedFieldName == "initial-pad-mapping")
    {
        setInitialPadMapping(initialPadMapping + i);
    }
    else if (focusedFieldName == "16-levels-erase-mode")
    {
        set16LevelsEraseMode(sixteenLevelsEraseMode + i);
    }
    else if (focusedFieldName == "auto-convert-wavs")
    {
        setAutoConvertWavs(autoConvertWavs + i);
    }
    else if (focusedFieldName == "name-typing-with-keyboard")
    {
        setNameTypingWithKeyboard(i > 0);
    }
}

void VmpcSettingsScreen::setInitialPadMapping(const int i)
{
    if (i < 0 || i > 1)
    {
        return;
    }

    initialPadMapping = i;

    displayInitialPadMapping();
}

void VmpcSettingsScreen::displayInitialPadMapping() const
{
    findField("initial-pad-mapping")
        ->setText(initialPadMappingNames[initialPadMapping]);
}

void VmpcSettingsScreen::display16LevelsEraseMode() const
{
    findField("16-levels-erase-mode")
        ->setText(sixteenLevelsEraseModeNames[sixteenLevelsEraseMode]);
}

void VmpcSettingsScreen::set16LevelsEraseMode(const int i)
{
    sixteenLevelsEraseMode = std::clamp(i, 0, 1);
    display16LevelsEraseMode();
}

void VmpcSettingsScreen::setAutoConvertWavs(const int i)
{
    autoConvertWavs = std::clamp(i, 0, 1);
    displayAutoConvertWavs();
}

void VmpcSettingsScreen::displayAutoConvertWavs() const
{
    findField("auto-convert-wavs")
        ->setText(autoConvertWavs == 1 ? "YES" : "ASK");
}

void VmpcSettingsScreen::displayNameTypingWithKeyboard() const
{
    findField("name-typing-with-keyboard")
        ->setText(nameTypingWithKeyboardEnabled ? "YES" : "NO");
}

void VmpcSettingsScreen::setNameTypingWithKeyboard(const bool shouldBeEnabled)
{
    nameTypingWithKeyboardEnabled = shouldBeEnabled;
    displayNameTypingWithKeyboard();
}

bool VmpcSettingsScreen::isNameTypingWithKeyboardEnabled() const
{
    return nameTypingWithKeyboardEnabled;
}
