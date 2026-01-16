#include "VmpcMidiPresetsScreen.hpp"

#include "Logger.hpp"
#include "Mpc.hpp"
#include "controller/ClientEventController.hpp"
#include "controller/ClientMidiEventController.hpp"

#include "lcdgui/Parameter.hpp"
#include "lcdgui/screens/dialog/FileExistsScreen.hpp"
#include "lcdgui/Label.hpp"
#include "lcdgui/screens/VmpcMidiScreen.hpp"
#include "lcdgui/ScreenId.hpp"
#include "lcdgui/screens/window/NameScreen.hpp"

#include "disk/AbstractDisk.hpp"
#include "input/midi/MidiControlPresetUtil.hpp"

using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui::screens::dialog;
using namespace mpc::lcdgui::screens::dialog2;
using namespace mpc::input::midi;

VmpcMidiPresetsScreen::VmpcMidiPresetsScreen(Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "vmpc-midi-presets", layerIndex)
{
    for (int i = 0; i < 4; i++)
    {
        const int y = 11 + i * 9;
        auto nameParam = std::make_shared<Parameter>(
            mpc, "", "name" + std::to_string(i), 23, y + 1, 16 * 6);
        addChild(nameParam);

        auto autoLoadParam = std::make_shared<Parameter>(
            mpc, "Auto-load:", "auto-load" + std::to_string(i), 23 + 17 * 6,
            y + 1, 3 * 6);
        addChild(autoLoadParam);
    }

    checkFileExistsAndSavePresetAndShowPopup = [this](std::string &presetName)
    {
        if (MidiControlPresetUtil::doesPresetWithNameExist(
                this->mpc.paths->getDocuments()->midiControlPresetsPath(),
                presetName))
        {
            auto replaceAction = [this, presetName]
            {
                const auto vmpcMidiScreen =
                    this->mpc.screens->get<ScreenId::VmpcMidiScreen>();

                const auto preset = vmpcMidiScreen->getActivePreset();
                preset->name = presetName;

                const auto presetPath =
                    this->mpc.paths->getDocuments()->midiControlPresetsPath() /
                    (preset->name + ".json");

                this->mpc.getDisk()->writeMidiControlPreset(preset, presetPath);

                ls.lock()->showPopupAndThenOpen(ScreenId::VmpcMidiPresetsScreen,
                                                "Saving " + presetName, 1000);
            };

            const auto initializeNameScreen = [this, presetName]
            {
                const auto nameScreen =
                    this->mpc.screens->get<ScreenId::NameScreen>();

                auto enterAction = [this](std::string &nameScreenName)
                {
                    this->checkFileExistsAndSavePresetAndShowPopup(
                        nameScreenName);
                };

                nameScreen->initialize(presetName, 16, enterAction,
                                       "vmpc-midi-presets");
            };

            const auto fileExistsScreen =
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
            const auto vmpcMidiScreen =
                this->mpc.screens->get<ScreenId::VmpcMidiScreen>();
            const auto preset = vmpcMidiScreen->getActivePreset();
            preset->name = presetName;
            const auto presetPath =
                this->mpc.paths->getDocuments()->midiControlPresetsPath() /
                (preset->name + ".json");
            this->mpc.getDisk()->writeMidiControlPreset(preset, presetPath);
            ls.lock()->showPopupAndThenOpen(ScreenId::VmpcMidiPresetsScreen,
                                            "Saving " + presetName, 1000);
        }
    };
}

void VmpcMidiPresetsScreen::open()
{
    presetMetas.clear();

    for (const auto entry : fs::directory_iterator(
             mpc.paths->getDocuments()->midiControlPresetsPath()))
    {
        const auto presetPath = entry.path();

        if (presetPath.extension() != ".json" ||
            presetPath ==
                mpc.paths->getDocuments()->activeMidiControlPresetPath())
        {
            continue;
        }

        const auto preset = std::make_shared<MidiControlPresetV3>();

        try
        {
            const auto presetFileData = get_file_data(presetPath);
            const auto presetJson = json::parse(presetFileData);
            from_json(presetJson, *preset);
        }
        catch (const std::exception &)
        {
            continue;
        }

        PresetMeta meta;
        meta.path = presetPath;
        meta.name = preset->name;
        meta.autoLoadMode = preset->autoLoadMode;
        presetMetas.emplace_back(meta);
    }

    if (row + rowOffset >= presetMetas.size() + 1)
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
    findChild<Label>("down")->Hide(rowOffset + 4 >= presetMetas.size());
}

void VmpcMidiPresetsScreen::turnWheel(const int i)
{
    const int presetIndex = row + rowOffset - 1;

    if (presetIndex < 0 || presetIndex >= presetMetas.size() || column == 0)
    {
        return;
    }

    auto candidate = std::clamp(presetMetas[presetIndex].autoLoadMode + i, 0,
                                AutoLoadModeCount - 1);

    if (presetMetas[presetIndex].autoLoadMode != candidate)
    {
        const auto preset = std::make_shared<MidiControlPresetV3>();

        try
        {
            const auto presetFileData =
                get_file_data(presetMetas[presetIndex].path);
            const auto presetJson = json::parse(presetFileData);
            from_json(presetJson, *preset);
        }
        catch (const std::exception &e)
        {
            ls.lock()->showPopupAndAwaitInteraction(
                "Error! See vmpc.log for info");

            MLOG(
                "Error while trying to change MIDI control preset auto-load "
                "mode. Pass the following information to the developer: " +
                std::string(e.what()));

            return;
        }

        presetMetas[presetIndex].autoLoadMode =
            static_cast<AutoLoadMode>(candidate);

        preset->autoLoadMode = static_cast<AutoLoadMode>(candidate);

        mpc.getDisk()->writeMidiControlPreset(preset,
                                              presetMetas[presetIndex].path);
    }

    displayRows();
}

void VmpcMidiPresetsScreen::function(const int i)
{
    ScreenComponent::function(i);

    switch (i)
    {
        case 2:
        {
            const int presetIndex = row + rowOffset - 1;

            const std::function enterAction =
                [this](std::string &nameScreenName)
            {
                checkFileExistsAndSavePresetAndShowPopup(nameScreenName);
            };

            if (presetIndex == -1)
            {
                const auto nameScreen =
                    mpc.screens->get<ScreenId::NameScreen>();
                nameScreen->initialize("New preset", 16, enterAction,
                                       "vmpc-midi-presets");
                openScreenById(ScreenId::NameScreen);
            }
            else
            {
                checkFileExistsAndSavePresetAndShowPopup(
                    presetMetas[presetIndex].name);
            }
            break;
        }
        case 3:
            openScreenById(ScreenId::VmpcMidiScreen);
            break;
        case 4:
        {
            const auto activePreset =
                mpc.clientEventController->getClientMidiEventController()
                    ->getExtendedController()
                    ->getActivePreset();

            const auto index = row + rowOffset - 1;

            if (index == -1)
            {
                MidiControlPresetUtil::resetMidiControlPreset(activePreset);
            }
            else
            {
                mpc.getDisk()->readMidiControlPreset(presetMetas[index].path,
                                                     activePreset);
            }

            ls.lock()->showPopupAndThenOpen(
                ScreenId::VmpcMidiPresetsScreen,
                "Loading " +
                    (index == -1 ? "Default" : presetMetas[index].name),
                700);
            break;
        }
        default:;
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
    if (row + rowOffset >= presetMetas.size())
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
    if (column == 1 || (row + rowOffset == 0 && presetMetas.empty()))
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
    for (int i = 0; i < 4; i++)
    {
        const int presetIndex = i + rowOffset - 1;
        const int presetCount = static_cast<int>(presetMetas.size());
        const auto name = findChild<Field>("name" + std::to_string(i));
        const auto autoLoadField =
            findChild<Field>("auto-load" + std::to_string(i));
        const auto autoLoadLabel =
            findChild<Label>("auto-load" + std::to_string(i));

        name->Hide(presetIndex >= presetCount);
        autoLoadField->Hide(presetIndex == -1 || presetIndex >= presetCount);
        autoLoadLabel->Hide(presetIndex == -1 || presetIndex >= presetCount);

        if (presetIndex >= presetCount)
        {
            continue;
        }

        const auto nameText =
            presetIndex == -1 ? "New preset" : presetMetas[presetIndex].name;

        name->setText(nameText);
        name->setInverted(i == row && column == 0);

        const auto autoloadText =
            presetIndex == -1
                ? ""
                : autoLoadModeToString(presetMetas[presetIndex].autoLoadMode);
        autoLoadField->setText(autoloadText);
        autoLoadField->setInverted(i == row && column == 1);
    }

    displayUpAndDown();
}
