#include "VmpcMidiPresetsScreen.hpp"

#include "lcdgui/Parameter.hpp"
#include "lcdgui/screens/dialog2/PopupScreen.hpp"
#include "nvram/MidiMappingPersistence.hpp"
#include "NameScreen.hpp"

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
    }

    saveMappingAndShowPopup = [this](std::string& newName1) {
        MidiMappingPersistence::saveMappingToFile(this->mpc, newName1);
        auto popupScreen = this->mpc.screens->get<PopupScreen>("popup");
        popupScreen->setText("Saving " + newName1);
        popupScreen->returnToScreenAfterMilliSeconds("vmpc-midi-presets", 1000);
    };
}

void VmpcMidiPresetsScreen::open()
{
    findChild<Label>("up")->setText("\u00C7");
    findChild<Label>("down")->setText("\u00C6");

    initPresets();
    displayRows();
}

void VmpcMidiPresetsScreen::displayUpAndDown()
{
    findChild<Label>("up")->Hide(rowOffset == 0);
    findChild<Label>("down")->Hide(rowOffset + 4 >= presets.size());
}

void VmpcMidiPresetsScreen::function(int i)
{
    ScreenComponent::function(i);

    switch (i)
    {
        case 2: {
            auto index = row + rowOffset;
            auto nameScreen = mpc.screens->get<NameScreen>("name");
            nameScreen->setNameLimit(16);
            nameScreen->setRenamerAndScreenToReturnTo(saveMappingAndShowPopup, "popup");

            if (index == 0) {
                nameScreen->setName("New preset");
                openScreen("name");
            }
            else
            {
                nameScreen->setName(presets[index]);
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
                MidiMappingPersistence::loadDefaultMapping(mpc);
            }
            else
            {
                MidiMappingPersistence::loadMappingFromFile(mpc, presets[index]);
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
    displayRows();
}

void VmpcMidiPresetsScreen::down()
{
    if (row + rowOffset + 1 >= presets.size()) return;
    if (row == 3) rowOffset++; else row++;
    displayRows();
}

void VmpcMidiPresetsScreen::displayRows()
{
    for (int i = 0; i < 4; i++)
    {
        auto name = findChild<Field>("name" + std::to_string(i));

        if (i >= presets.size())
        {
            name->setText("");
            continue;
        }

        name->setText(presets[i + rowOffset]);
        name->setInverted(i == row);
    }

    displayUpAndDown();
}

void VmpcMidiPresetsScreen::initPresets()
{
    presets.clear();
    presets.emplace_back("New preset");

    for (auto& name : MidiMappingPersistence::getAvailablePresetNames())
    {
        presets.emplace_back(name);
    }
}