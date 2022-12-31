#include "VmpcMidiPresetsScreen.hpp"

#include "lcdgui/Parameter.hpp"
#include "lcdgui/screens/dialog2/PopupScreen.hpp"
#include "lcdgui/screens/dialog/FileExistsScreen.hpp"
#include "nvram/MidiControlPersistence.hpp"
#include "disk/AbstractDisk.hpp"
#include "NameScreen.hpp"
#include "lcdgui/screens/VmpcMidiScreen.hpp"

using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui::screens::dialog;
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

        auto autoLoadParam = std::make_shared<Parameter>(mpc, "Auto-load:", "auto-load" + std::to_string(i),
                                                         23 + (17 * 6), y + 1, 3 * 6);
        addChild(autoLoadParam);
    }

    checkFileExistsAndSavePresetAndShowPopup = [this](std::string& presetName) {

        if (MidiControlPersistence::doesPresetWithNameExist(presetName))
        {
            auto replaceAction = [this, presetName]{
                auto vmpcMidiScreen = this->mpc.screens->get<VmpcMidiScreen>("vmpc-midi");
                auto preset = vmpcMidiScreen->getActivePreset();
                preset->name = presetName;
                this->mpc.getDisk()->writeMidiControlPreset(preset);
                MidiControlPersistence::loadAllPresetsFromDiskIntoMemory(this->mpc);
                auto popupScreen = this->mpc.screens->get<PopupScreen>("popup");
                popupScreen->setText("Saving " + presetName);
                popupScreen->returnToScreenAfterMilliSeconds("vmpc-midi-presets", 1000);
                openScreen("popup");
            };

            const auto initializeNameScreen = [this, presetName]{
                auto nameScreen = this->mpc.screens->get<NameScreen>("name");

                auto enterAction = [this](std::string& nameScreenName){
                    this->checkFileExistsAndSavePresetAndShowPopup(nameScreenName);
                };

                nameScreen->initialize(presetName, 16, enterAction, this->name);
            };

            auto fileExistsScreen = this->mpc.screens->get<FileExistsScreen>("file-exists");
            fileExistsScreen->initialize(replaceAction, initializeNameScreen, name);
            openScreen("file-exists");
        }
        else
        {
            auto vmpcMidiScreen = this->mpc.screens->get<VmpcMidiScreen>("vmpc-midi");
            auto preset = vmpcMidiScreen->getActivePreset();
            preset->name = presetName;
            this->mpc.getDisk()->writeMidiControlPreset(preset);
            MidiControlPersistence::loadAllPresetsFromDiskIntoMemory(this->mpc);
            auto popupScreen = this->mpc.screens->get<PopupScreen>("popup");
            popupScreen->setText("Saving " + presetName);
            popupScreen->returnToScreenAfterMilliSeconds("vmpc-midi-presets", 1000);
            openScreen("popup");
        }
    };
}

void VmpcMidiPresetsScreen::open()
{
    MidiControlPersistence::loadAllPresetsFromDiskIntoMemory(mpc);

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

    auto candidate = presets[presetIndex]->autoloadMode + i;

    if (candidate < 0) candidate = 0;
    else if (candidate > 2) candidate = 2;

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

    auto& presets = MidiControlPersistence::presets;

    switch (i)
    {
        case 2: {
            const int presetIndex = (row + rowOffset) - 1;

            std::function<void(std::string&)> enterAction;

            enterAction = [this](std::string& nameScreenName) {
                checkFileExistsAndSavePresetAndShowPopup(nameScreenName);
            };

            if (presetIndex == -1)
            {
                auto nameScreen = mpc.screens->get<NameScreen>("name");
                nameScreen->initialize("New preset", 16, enterAction, name);
                openScreen("name");
            }
            else
            {
                checkFileExistsAndSavePresetAndShowPopup(presets[presetIndex]->name);
            }
            break;
        }
        case 3:
            openScreen("vmpc-midi");
            break;
        case 4: {
            auto index = (row + rowOffset) - 1;

            if (index == -1)
            {
                MidiControlPersistence::loadDefaultMapping(mpc);
            }
            else
            {
                auto vmpcMidiScreen = mpc.screens->get<VmpcMidiScreen>("vmpc-midi");
                MidiControlPersistence::loadFileByNameIntoPreset(mpc, presets[index]->name, vmpcMidiScreen->getActivePreset());
            }

            auto popupScreen = mpc.screens->get<PopupScreen>("popup");
            popupScreen->setText("Loading " + (index == -1 ? "Default" : presets[index]->name));
            popupScreen->returnToScreenAfterMilliSeconds("vmpc-midi-presets", 700);
            mpc.getLayeredScreen()->openScreen("popup");
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

        const auto nameText = presetIndex == -1 ? "New preset" : presets[presetIndex]->name;

        name->setText(nameText);
        name->setInverted(i == row && column == 0);

        const auto autoloadText = presetIndex == -1 ? "" : autoLoadModeNames[presets[presetIndex]->autoloadMode];
        autoLoadField->setText(autoloadText);
        autoLoadField->setInverted(i == row && column == 1);
    }

    displayUpAndDown();
}
