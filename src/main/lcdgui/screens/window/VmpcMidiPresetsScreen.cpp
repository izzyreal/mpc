#include "VmpcMidiPresetsScreen.hpp"

#include "Mpc.hpp"
#include "lcdgui/Parameter.hpp"
#include "lcdgui/screens/dialog/FileExistsScreen.hpp"
#include "nvram/MidiControlPersistence.hpp"
#include "disk/AbstractDisk.hpp"
#include "NameScreen.hpp"
#include "lcdgui/Label.hpp"
#include "lcdgui/screens/VmpcMidiScreen.hpp"
#include "lcdgui/ScreenId.hpp"

using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui::screens::dialog;
using namespace mpc::lcdgui::screens::dialog2;
using namespace mpc::nvram;

VmpcMidiPresetsScreen::VmpcMidiPresetsScreen(mpc::Mpc &mpc,
                                             const int layerIndex)
    : ScreenComponent(mpc, "vmpc-midi-presets", layerIndex)
{
    for (int i = 0; i < 4; i++)
    {
        const int y = 11 + (i * 9);
        auto nameParam = std::make_shared<Parameter>(
            mpc, "", "name" + std::to_string(i), 23, y + 1, 16 * 6);
        addChild(nameParam);

        auto autoLoadParam = std::make_shared<Parameter>(
            mpc, "Auto-load:", "auto-load" + std::to_string(i), 23 + (17 * 6),
            y + 1, 3 * 6);
        addChild(autoLoadParam);
    }

    checkFileExistsAndSavePresetAndShowPopup = [this](std::string &presetName)
    {
        if (MidiControlPersistence::doesPresetWithNameExist(this->mpc,
                                                            presetName))
        {
            auto replaceAction = [this, presetName]
            {
                auto vmpcMidiScreen =
                    this->mpc.screens->get<ScreenId::VmpcMidiScreen>();
                auto preset = vmpcMidiScreen->getActivePreset();
                preset->name = presetName;
                this->mpc.getDisk()->writeMidiControlPreset(preset);
                MidiControlPersistence::loadAllPresetsFromDiskIntoMemory(
                    this->mpc);
                ls->showPopupAndThenOpen<ScreenId::VmpcMidiPresetsScreen>(
                    "Saving " + presetName, 1000);
            };

            const auto initializeNameScreen = [this, presetName]
            {
                auto nameScreen =
                    this->mpc.screens->get<ScreenId::NameScreen>();

                auto enterAction = [this](std::string &nameScreenName)
                {
                    this->checkFileExistsAndSavePresetAndShowPopup(
                        nameScreenName);
                };

                nameScreen->initialize(presetName, 16, enterAction,
                                       "vmpc-midi-presets");
            };

            auto fileExistsScreen =
                this->mpc.screens->get<ScreenId::FileExistsScreen>();
            fileExistsScreen->initialize(
                replaceAction, initializeNameScreen,
                [this]
                {
                    this->openScreenById(ScreenId::VmpcMidiPresetsScreen);
                });
            this->openScreenById(ScreenId::FileExistsScreen);
        }
        else
        {
            auto vmpcMidiScreen =
                this->mpc.screens->get<ScreenId::VmpcMidiScreen>();
            auto preset = vmpcMidiScreen->getActivePreset();
            preset->name = presetName;
            this->mpc.getDisk()->writeMidiControlPreset(preset);
            MidiControlPersistence::loadAllPresetsFromDiskIntoMemory(this->mpc);
            ls->showPopupAndThenOpen<ScreenId::VmpcMidiPresetsScreen>(
                "Saving " + presetName, 1000);
        }
    };
}

void VmpcMidiPresetsScreen::open()
{
    MidiControlPersistence::loadAllPresetsFromDiskIntoMemory(mpc);

    if (row + rowOffset >= (mpc.midiControlPresets.size() + 1))
    {
        row = 0;
        rowOffset = 0;
    }

    findChild<Label>("up")->setText("\u00C7");
    findChild<Label>("down")->setText("\u00C6");

    displayRows();
}

void VmpcMidiPresetsScreen::displayUpAndDown()
{
    findChild<Label>("up")->Hide(rowOffset == 0);
    findChild<Label>("down")->Hide(rowOffset + 4 >=
                                   mpc.midiControlPresets.size());
}

void VmpcMidiPresetsScreen::turnWheel(int i)
{
    auto &presets = mpc.midiControlPresets;
    const int presetIndex = (row + rowOffset) - 1;

    if (presetIndex < 0 || presetIndex >= presets.size())
    {
        return;
    }

    auto candidate = presets[presetIndex]->autoloadMode + i;

    if (candidate < 0)
    {
        candidate = 0;
    }
    else if (candidate > 2)
    {
        candidate = 2;
    }

    if (presets[presetIndex]->autoloadMode != candidate)
    {
        presets[presetIndex]->autoloadMode = candidate;
        mpc.getDisk()->writeMidiControlPreset(presets[presetIndex]);
        nvram::MidiControlPersistence::loadAllPresetsFromDiskIntoMemory(mpc);
    }

    displayRows();
}

void VmpcMidiPresetsScreen::function(int i)
{
    ScreenComponent::function(i);

    auto &presets = mpc.midiControlPresets;

    switch (i)
    {
        case 2:
        {
            const int presetIndex = (row + rowOffset) - 1;

            std::function<void(std::string &)> enterAction;

            enterAction = [this](std::string &nameScreenName)
            {
                checkFileExistsAndSavePresetAndShowPopup(nameScreenName);
            };

            if (presetIndex == -1)
            {
                auto nameScreen = mpc.screens->get<ScreenId::NameScreen>();
                nameScreen->initialize("New preset", 16, enterAction,
                                       "vmpc-midi-presets");
                openScreenById(ScreenId::NameScreen);
            }
            else
            {
                checkFileExistsAndSavePresetAndShowPopup(
                    presets[presetIndex]->name);
            }
            break;
        }
        case 3:
            openScreenById(ScreenId::VmpcMidiScreen);
            break;
        case 4:
        {
            auto index = (row + rowOffset) - 1;

            if (index == -1)
            {
                MidiControlPersistence::loadDefaultMapping(mpc);
            }
            else
            {
                auto vmpcMidiScreen =
                    mpc.screens->get<ScreenId::VmpcMidiScreen>();
                MidiControlPersistence::loadFileByNameIntoPreset(
                    mpc, presets[index]->name,
                    vmpcMidiScreen->getActivePreset());
            }

            ls->showPopupAndThenOpen<ScreenId::VmpcMidiPresetsScreen>(
                "Loading " + (index == -1 ? "Default" : presets[index]->name),
                700);
            break;
        }
    }
}

void VmpcMidiPresetsScreen::up()
{
    if (row == 0 && rowOffset == 0)
    {
        return;
    }
    if (row == 0)
    {
        rowOffset--;
    }
    else
    {
        row--;
    }
    if (row + rowOffset == 0)
    {
        column = 0;
    }
    displayRows();
}

void VmpcMidiPresetsScreen::down()
{
    if (row + rowOffset >= mpc.midiControlPresets.size())
    {
        return;
    }
    if (row == 3)
    {
        rowOffset++;
    }
    else
    {
        row++;
    }
    displayRows();
}

void VmpcMidiPresetsScreen::left()
{
    if (column == 0)
    {
        return;
    }
    column--;
    displayRows();
}

void VmpcMidiPresetsScreen::right()
{
    if (column == 1 || (row + rowOffset == 0 && mpc.midiControlPresets.empty()))
    {
        return;
    }

    if (row + rowOffset == 0)
    {
        row++;
    }

    column++;
    displayRows();
}

void VmpcMidiPresetsScreen::displayRows()
{
    auto presets = mpc.midiControlPresets;

    for (int i = 0; i < 4; i++)
    {
        const int presetIndex = (i + rowOffset) - 1;
        const int presetCount = static_cast<int>(presets.size());
        auto name = findChild<Field>("name" + std::to_string(i));
        auto autoLoadField = findChild<Field>("auto-load" + std::to_string(i));
        auto autoLoadLabel = findChild<Label>("auto-load" + std::to_string(i));

        name->Hide(presetIndex >= presetCount);
        autoLoadField->Hide(presetIndex == -1 || presetIndex >= presetCount);
        autoLoadLabel->Hide(presetIndex == -1 || presetIndex >= presetCount);

        if (presetIndex >= presetCount)
        {
            continue;
        }

        const auto nameText =
            presetIndex == -1 ? "New preset" : presets[presetIndex]->name;

        name->setText(nameText);
        name->setInverted(i == row && column == 0);

        const auto autoloadText =
            presetIndex == -1
                ? ""
                : autoLoadModeNames[presets[presetIndex]->autoloadMode];
        autoLoadField->setText(autoloadText);
        autoLoadField->setInverted(i == row && column == 1);
    }

    displayUpAndDown();
}
