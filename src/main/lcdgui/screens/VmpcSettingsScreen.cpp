#include "VmpcSettingsScreen.hpp"
#include "lcdgui/LayeredScreen.hpp"

using namespace mpc::lcdgui::screens;

VmpcSettingsScreen::VmpcSettingsScreen(mpc::Mpc &mpc, const int layerIndex)
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
    displayMidiControlMode();
    displayNameTypingWithKeyboard();
    ls->setFunctionKeysArrangement(
        midiControlMode == MidiControlMode::ORIGINAL ? 1 : 0);
}

void VmpcSettingsScreen::close()
{
    if (!easterEgg->IsHidden())
    {
        easterEgg->setScrolling(false);
        easterEgg->Hide(true);
    }
}

void VmpcSettingsScreen::function(int i)
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
            if (midiControlMode == MidiControlMode::ORIGINAL)
            {
                return;
            }

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
    }
}

void VmpcSettingsScreen::turnWheel(int i)
{

    const auto focusedFieldName = getFocusedFieldName();

    if (focusedFieldName == "initial-pad-mapping")
    {
        setInitialPadMapping(initialPadMapping + i);
    }
    else if (focusedFieldName == "16-levels-erase-mode")
    {
        set16LevelsEraseMode(_16LevelsEraseMode + i);
    }
    else if (focusedFieldName == "auto-convert-wavs")
    {
        setAutoConvertWavs(autoConvertWavs + i);
    }
    else if (focusedFieldName == "midi-control-mode")
    {
        setMidiControlMode(midiControlMode + i);
        ls->setFunctionKeysArrangement(
            midiControlMode == MidiControlMode::ORIGINAL ? 1 : 0);
    }
    else if (focusedFieldName == "name-typing-with-keyboard")
    {
        setNameTypingWithKeyboard(i > 0);
    }
}

void VmpcSettingsScreen::setInitialPadMapping(int i)
{
    if (i < 0 || i > 1)
    {
        return;
    }

    initialPadMapping = i;

    displayInitialPadMapping();
}

void VmpcSettingsScreen::displayInitialPadMapping()
{
    findField("initial-pad-mapping")
        ->setText(initialPadMappingNames[initialPadMapping]);
}

void VmpcSettingsScreen::display16LevelsEraseMode()
{
    findField("16-levels-erase-mode")
        ->setText(_16LevelsEraseModeNames[_16LevelsEraseMode]);
}

void VmpcSettingsScreen::set16LevelsEraseMode(int i)
{
    if (i < 0 || i > 1)
    {
        return;
    }

    _16LevelsEraseMode = i;
    display16LevelsEraseMode();
}

void VmpcSettingsScreen::setAutoConvertWavs(int i)
{
    if (i < 0 || i > 1)
    {
        return;
    }

    autoConvertWavs = i;
    displayAutoConvertWavs();
}

void VmpcSettingsScreen::displayAutoConvertWavs()
{
    findField("auto-convert-wavs")
        ->setText(autoConvertWavs == 1 ? "YES" : "ASK");
}

void VmpcSettingsScreen::setMidiControlMode(int i)
{
    if (i < 0 || i > 1)
    {
        return;
    }

    midiControlMode = i;
    displayMidiControlMode();
}

void VmpcSettingsScreen::displayMidiControlMode()
{
    findField("midi-control-mode")
        ->setText(midiControlModeNames[midiControlMode]);
}

int VmpcSettingsScreen::getMidiControlMode()
{
    return midiControlMode;
}

void VmpcSettingsScreen::displayNameTypingWithKeyboard()
{
    findField("name-typing-with-keyboard")
        ->setText(nameTypingWithKeyboardEnabled ? "YES" : "NO");
}

void VmpcSettingsScreen::setNameTypingWithKeyboard(const bool shouldBeEnabled)
{
    nameTypingWithKeyboardEnabled = shouldBeEnabled;
    displayNameTypingWithKeyboard();
}

const bool VmpcSettingsScreen::isNameTypingWithKeyboardEnabled()
{
    return nameTypingWithKeyboardEnabled;
}
