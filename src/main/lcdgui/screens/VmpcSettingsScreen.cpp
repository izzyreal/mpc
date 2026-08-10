#include "VmpcSettingsScreen.hpp"
#include "Mpc.hpp"
#include "engine/EngineHost.hpp"
#include "lcdgui/LayeredScreen.hpp"
#include "lcdgui/Field.hpp"
#include "lcdgui/Label.hpp"
#include "lcdgui/screens/window/TimingCorrectScreen.hpp"
#include "sampler/Pad.hpp"
#include "sampler/Sampler.hpp"

#include <algorithm>

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
    findChild<Label>("up")->setText("\u00C7");
    findChild<Label>("down")->setText("\u00C6");

    if (const auto focusedFieldName = getFocusedFieldName())
    {
        const auto focused = std::find(settingNames.begin(), settingNames.end(),
                                       *focusedFieldName);
        if (focused != settingNames.end())
        {
            const auto selected =
                static_cast<int>(std::distance(settingNames.begin(), focused));
            rowOffset = std::clamp(selected - VisibleRowCount + 1, 0,
                                   SettingCount - VisibleRowCount);
            row = selected - rowOffset;
        }
    }
    displayRows();
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

void VmpcSettingsScreen::openWindow()
{
    const auto selectedSetting = row + rowOffset;
    if (selectedSetting >= 5 && selectedSetting <= 7)
    {
        openScreenById(ScreenId::VmpcPhysicalSoundsScreen);
        return;
    }
    ScreenComponent::openWindow();
}

void VmpcSettingsScreen::turnWheel(const int i)
{
    switch (row + rowOffset)
    {
        case 0:
            setInitialPadMapping(initialPadMapping + i);
            break;
        case 1:
            set16LevelsEraseMode(sixteenLevelsEraseMode + i);
            break;
        case 2:
            setAutoConvertWavs(autoConvertWavs + i);
            break;
        case 3:
            setNameTypingWithKeyboard(i > 0);
            break;
        case 4:
            setBigTimeShift(i > 0);
            break;
        case 5:
            setPhysicalSoundsEnabled(i > 0);
            break;
        case 6:
            setPhysicalSoundsMixMode(physicalSoundsMixMode + i);
            break;
        case 7:
            setPhysicalSoundsLevel(physicalSoundsLevel + i);
            break;
        default:
            break;
    }
}

void VmpcSettingsScreen::up()
{
    const auto selected = row + rowOffset;
    if (selected > 0)
    {
        selectSetting(selected - 1);
    }
}

void VmpcSettingsScreen::down()
{
    const auto selected = row + rowOffset;
    if (selected + 1 < SettingCount)
    {
        selectSetting(selected + 1);
    }
}

void VmpcSettingsScreen::selectSetting(const int settingIndex)
{
    auto nextRowOffset = rowOffset;
    if (settingIndex < nextRowOffset)
    {
        nextRowOffset = settingIndex;
    }
    else if (settingIndex >= nextRowOffset + VisibleRowCount)
    {
        nextRowOffset = settingIndex - VisibleRowCount + 1;
    }

    const auto viewportMoved = nextRowOffset != rowOffset;
    const auto nextField = findField(settingNames[settingIndex]);
    nextField->Hide(false);
    ls.lock()->setFocus(settingNames[settingIndex]);

    rowOffset = nextRowOffset;
    row = settingIndex - rowOffset;
    displayRows();

    if (viewportMoved)
    {
        // A newly revealed row may move out of the area occupied by the
        // function keys. Redraw the whole screen after clearing its old
        // rectangle so those static controls are restored as well.
        SetDirty();
    }
}

void VmpcSettingsScreen::setInitialPadMapping(const int i)
{
    if (i < 0 || i > 1)
    {
        return;
    }

    initialPadMapping = i;
    // Future PROGRAM/MASTER init-pad-assign operations copy from this cache.
    *mpc.getSampler()->getInitMasterPadAssign() =
        sampler::Pad::getPadNotes(mpc);

    displayInitialPadMapping();
}

void VmpcSettingsScreen::displayInitialPadMapping() const
{
    displayRows();
}

void VmpcSettingsScreen::display16LevelsEraseMode() const
{
    displayRows();
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
    displayRows();
}

void VmpcSettingsScreen::displayNameTypingWithKeyboard() const
{
    displayRows();
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

void VmpcSettingsScreen::displayBigTimeShift() const
{
    displayRows();
}

void VmpcSettingsScreen::setBigTimeShift(const bool shouldBeEnabled)
{
    bigTimeShiftEnabled = shouldBeEnabled;
    displayBigTimeShift();
    mpc.screens->get<ScreenId::TimingCorrectScreen>()
        ->reClampAmountToCurrentMode();
}

bool VmpcSettingsScreen::isBigTimeShiftEnabled() const
{
    return bigTimeShiftEnabled;
}

void VmpcSettingsScreen::setPhysicalSoundsEnabled(const bool shouldBeEnabled)
{
    physicalSoundsEnabled = shouldBeEnabled;
    mpc.getEngineHost()->setPhysicalSoundsEnabled(shouldBeEnabled);
    displayRows();
}

bool VmpcSettingsScreen::arePhysicalSoundsEnabled() const
{
    return physicalSoundsEnabled;
}

void VmpcSettingsScreen::setPhysicalSoundsMixMode(const int mode)
{
    physicalSoundsMixMode = std::clamp(mode, 0, 1);
    mpc.getEngineHost()->setPhysicalSoundsMixMode(
        physicalSoundsMixMode == 0 ? engine::PhysicalSoundsMixMode::StereoOut
                                   : engine::PhysicalSoundsMixMode::Dedicated);
    displayRows();
}

int VmpcSettingsScreen::getPhysicalSoundsMixMode() const
{
    return physicalSoundsMixMode;
}

void VmpcSettingsScreen::setPhysicalSoundsLevel(const int level)
{
    physicalSoundsLevel = std::clamp(level, 0, 100);
    mpc.getEngineHost()->setPhysicalSoundsLevel(physicalSoundsLevel);
    displayRows();
}

int VmpcSettingsScreen::getPhysicalSoundsLevel() const
{
    return physicalSoundsLevel;
}

std::string VmpcSettingsScreen::getSettingValue(const int settingIndex) const
{
    switch (settingIndex)
    {
        case 0:
            return initialPadMappingNames[initialPadMapping];
        case 1:
            return sixteenLevelsEraseModeNames[sixteenLevelsEraseMode];
        case 2:
            return autoConvertWavs == 1 ? "YES" : "ASK";
        case 3:
            return nameTypingWithKeyboardEnabled ? "YES" : "NO";
        case 4:
            return bigTimeShiftEnabled ? "YES" : "NO (ORIGINAL)";
        case 5:
            return physicalSoundsEnabled ? "YES" : "NO";
        case 6:
            return physicalSoundsMixMode == 0 ? "STEREO OUT" : "PHYSICAL BUS";
        case 7:
            return std::to_string(physicalSoundsLevel);
        default:
            return {};
    }
}

void VmpcSettingsScreen::displayRows() const
{
    for (int settingIndex = 0; settingIndex < SettingCount; ++settingIndex)
    {
        const auto label = findLabel(settingNames[settingIndex]);
        const auto field = findField(settingNames[settingIndex]);
        const auto isVisible = settingIndex >= rowOffset &&
                               settingIndex < rowOffset + VisibleRowCount;
        label->Hide(!isVisible);
        field->Hide(!isVisible);
        if (!isVisible)
        {
            continue;
        }

        const auto visibleRow = settingIndex - rowOffset;
        label->setLocation(label->getX(), 2 + visibleRow * 9);
        field->setLocation(field->getX(), 2 + visibleRow * 9);
        label->setText(settingLabels[settingIndex]);
        field->setText(getSettingValue(settingIndex));
    }
    displayUpAndDown();
}

void VmpcSettingsScreen::displayUpAndDown() const
{
    findLabel("up")->Hide(rowOffset == 0);
    findLabel("down")->Hide(rowOffset + VisibleRowCount >= SettingCount);
}
