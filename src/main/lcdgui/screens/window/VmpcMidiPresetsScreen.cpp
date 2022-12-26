#include "VmpcMidiPresetsScreen.hpp"

#include "lcdgui/Parameter.hpp"
#include "lcdgui/screens/dialog2/PopupScreen.hpp"
#include "nvram/MidiControlPersistence.hpp"
#include "NameScreen.hpp"
#include "lcdgui/screens/VmpcMidiScreen.hpp"

using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui::screens::dialog2;
using namespace mpc::nvram;

VmpcMidiPresetsScreen::VmpcMidiPresetsScreen(mpc::Mpc& mpc, const int layerIndex)
        : ScreenComponent(mpc, "vmpc-midi-presets", layerIndex)
{
    for (int i = 0; i < 4; i++)
    {
        const int y = 11 + (i * 9);
        auto nameParam = std::make_shared<Parameter>(mpc, "", "name" + std::to_string(i), 23, y + 1, 16 * 6);
        addChild(nameParam);

        auto autoLoadParam = std::make_shared<Parameter>(mpc, "Auto-load:", "auto-load" + std::to_string(i), 23 + (17 * 6), y + 1, 3 * 6);
        addChild(autoLoadParam);
    }

    saveMappingAndShowPopup = [this](std::string& newName1) {
        MidiControlPersistence::saveCurrentMappingToFile(this->mpc, newName1, MidiControlPreset::AutoLoadMode::ASK);
        auto popupScreen = this->mpc.screens->get<PopupScreen>("popup");
        popupScreen->setText("Saving " + newName1);
        popupScreen->returnToScreenAfterMilliSeconds("vmpc-midi-presets", 1000);
    };
}

void VmpcMidiPresetsScreen::open()
{
    MidiControlPersistence::loadAllPresetsFromDiskIntoMemory();

    if (row + rowOffset >= (MidiControlPersistence::presets.size() + 1))
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
    findChild<Label>("down")->Hide(rowOffset + 4 >= MidiControlPersistence::presets.size());
}

void VmpcMidiPresetsScreen::turnWheel(int i)
{
    init();
    auto &presets = MidiControlPersistence::presets;
    const int presetIndex = (row + rowOffset) - 1;

    if (presetIndex < 0 || presetIndex >= presets.size())
    {
        return;
    }

    auto candidate = presets[presetIndex].autoloadMode + i;

    if (candidate < 0) candidate = 0;
    else if (candidate > 2) candidate = 2;

    if (presets[presetIndex].autoloadMode != candidate)
    {
        presets[presetIndex].autoloadMode = candidate;
        MidiControlPersistence::savePresetToFile(presets[presetIndex]);
    }

    displayRows();
}

void VmpcMidiPresetsScreen::function(int i)
{
    ScreenComponent::function(i);

    auto& presets = MidiControlPersistence::presets;

    switch (i)
    {
        case 2: {
            const int presetIndex = (row + rowOffset) - 1;
            auto nameScreen = mpc.screens->get<NameScreen>("name");
            nameScreen->setNameLimit(16);
            nameScreen->setRenamerAndScreenToReturnTo(saveMappingAndShowPopup, "popup");

            if (presetIndex == -1) {
                nameScreen->setName("New preset");
                openScreen("name");
            }
            else
            {
                nameScreen->setName(presets[presetIndex].name);
                openScreen("file-exists");
            }
            break;
        }
        case 3:
            openScreen("vmpc-midi");
            break;
        case 4: {
            auto index = row + rowOffset;

            if (index == 0)
            {
                MidiControlPersistence::loadDefaultMapping(mpc);
            }
            else
            {
                auto vmpcMidiScreen = mpc.screens->get<VmpcMidiScreen>("vmpc-midi");
                MidiControlPersistence::loadFileByNameIntoPreset(presets[index].name, vmpcMidiScreen->getActivePreset());
            }

            openScreen("vmpc-midi");
            break;
        }
    }
}

void VmpcMidiPresetsScreen::up()
{
    if (row == 0 && rowOffset == 0) return;
    if (row == 0) rowOffset--; else row--;
    if (row + rowOffset == 0) column = 0;
    displayRows();
}

void VmpcMidiPresetsScreen::down()
{
    if (row + rowOffset >= MidiControlPersistence::presets.size()) return;
    if (row == 3) rowOffset++; else row++;
    displayRows();
}

void VmpcMidiPresetsScreen::left()
{
    if (column == 0) return;
    column--;
    displayRows();
}

void VmpcMidiPresetsScreen::right()
{
    if (column == 1 || (row + rowOffset == 0 && MidiControlPersistence::presets.size() == 0)) return;

    if (row + rowOffset == 0)
    {
        row++;
    }

    column++;
    displayRows();
}

void VmpcMidiPresetsScreen::displayRows()
{
    auto presets = MidiControlPersistence::presets;

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

        if (presetIndex >= presetCount) continue;

        const auto nameText = presetIndex == -1 ? "New preset" : presets[presetIndex].name;

        name->setText(nameText);
        name->setInverted(i == row && column == 0);

        const auto autoloadText = presetIndex == -1 ? "" : autoLoadModeNames[presets[presetIndex].autoloadMode];
        autoLoadField->setText(autoloadText);
        autoLoadField->setInverted(i == row && column == 1);
    }

    displayUpAndDown();
}

int VmpcMidiPresetsScreen::getActivePresetIndex()
{
    return (row + rowOffset) - 1;
}
