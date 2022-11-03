#include "VmpcMidiPresetsScreen.hpp"

#include "lcdgui/Parameter.hpp"
#include "nvram/MidiMappingPersistence.hpp"

using namespace mpc::lcdgui::screens::window;

VmpcMidiPresetsScreen::VmpcMidiPresetsScreen(mpc::Mpc& mpc, const int layerIndex)
        : ScreenComponent(mpc, "vmpc-midi-presets", layerIndex)
{
    for (int i = 0; i < 4; i++)
    {
        const int y = 11 + (i * 9);
        auto nameParam = std::make_shared<Parameter>(mpc, "", "name" + std::to_string(i), 23, y + 1, 32 * 6);
        addChild(nameParam);
    }
}

void VmpcMidiPresetsScreen::open()
{
    initPresets();
    displayRows();
}

void VmpcMidiPresetsScreen::displayUpAndDown()
{
    findChild<Label>("up").lock()->Hide(rowOffset == 0);
    findChild<Label>("down").lock()->Hide(rowOffset + 4 >= presets.size());
}

void VmpcMidiPresetsScreen::function(int i)
{
    ScreenComponent::function(i);

    switch (i)
    {
        case 2:
            // save
            break;
        case 3:
            openScreen("vmpc-midi");
            break;
        case 4:
            auto index = row + rowOffset;
            if (index == 0)
            {
                mpc::nvram::MidiMappingPersistence::loadDefaultMapping(mpc);
            }
            break;
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
        auto name = findChild<Field>("name" + std::to_string(i)).lock();

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
}