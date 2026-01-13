#include "VmpcMidiScreen.hpp"

#include "Mpc.hpp"

#include "lcdgui/screens/window/VmpcDiscardMappingChangesScreen.hpp"

#include "lcdgui/Parameter.hpp"
#include "lcdgui/Label.hpp"
#include "lcdgui/Field.hpp"

#include "controller/ClientEventController.hpp"
#include "controller/ClientMidiEventController.hpp"
#include "controller/ClientExtendedMidiController.hpp"

#include "StrUtil.hpp"

using namespace mpc::input::midi;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui::screens::dialog2;
using namespace mpc::lcdgui;

VmpcMidiScreen::VmpcMidiScreen(Mpc &mpc, int layerIndex)
    : ScreenComponent(mpc, "vmpc-midi", layerIndex)
{
    for (int i = 0; i < 5; i++)
    {
        auto typeParam = std::make_shared<Parameter>(mpc, "                ",
                                                     "type" + std::to_string(i),
                                                     2, 3 + i * 9, 4 * 6);
        addChild(typeParam);

        auto numberParam = std::make_shared<Parameter>(
            mpc, "#", "number" + std::to_string(i), 125, 3 + i * 9, 3 * 6);
        addChild(numberParam);

        auto valueParam = std::make_shared<Parameter>(
            mpc, "v:", "value" + std::to_string(i), 152, 3 + i * 9, 3 * 6);
        addChild(valueParam);

        auto channelParam = std::make_shared<Parameter>(
            mpc, "c:", "channel" + std::to_string(i), 190, 3 + i * 9, 3 * 6);
        addChild(channelParam);
    }
}

void VmpcMidiScreen::turnWheel(int i)
{
    Binding &binding = getActivePreset()->getBindingByIndex(row + rowOffset);

    if (column == 0)
    {
        binding.setMessageType(i > 0 ? BindingMessageType::Note
                                     : BindingMessageType::Controller);
    }
    else if (column == 1)
    {
        binding.setMidiNumber(std::clamp(binding.getMidiNumber() + i, -1, 127));
    }
    else if (column == 2)
    {
        binding.setMidiValue(std::clamp(binding.getMidiValue() + i, -1, 127));
    }
    else if (column == 3)
    {
        binding.setMidiChannelIndex(
            std::clamp(binding.getMidiChannelIndex() + i, -1, 15));
    }

    updateRows();
}

void VmpcMidiScreen::open()
{
    auto screen = mpc.screens->get<ScreenId::VmpcDiscardMappingChangesScreen>();

    screen->discardAndLeave = [this]()
    {
        auto controller =
            mpc.clientEventController->getClientMidiEventController()
                ->getExtendedController();

        auto activePreset = controller->getActivePreset();

        activePreset->setBindings(uneditedActivePresetCopy->getBindings());

        uneditedActivePresetCopy = std::make_shared<MidiControlPresetV3>();
    };

    screen->saveAndLeave = [this]()
    {
        uneditedActivePresetCopy = std::make_shared<MidiControlPresetV3>();
    };

    screen->stayScreen = "vmpc-midi";

    if (ls.lock()->isPreviousScreenNot(
            {ScreenId::VmpcDiscardMappingChangesScreen}))
    {
        uneditedActivePresetCopy = std::make_shared<MidiControlPresetV3>();

        auto controller =
            mpc.clientEventController->getClientMidiEventController()
                ->getExtendedController();

        auto activePreset = controller->getActivePreset();

        uneditedActivePresetCopy->setBindings(activePreset->getBindings());
    }

    findChild<Label>("up")->setText("\u00C7");
    findChild<Label>("down")->setText("\u00C6");

    setLearning(false);
    learnCandidate.reset();
    updateRows();
}

void VmpcMidiScreen::up()
{
    if (learning)
    {
        acceptLearnCandidate();
        learnCandidate.reset();
    }

    if (row == 0)
    {
        if (rowOffset == 0)
        {
            return;
        }

        rowOffset--;

        if (getActivePreset()->getBindingByIndex(row + rowOffset).isNote() &&
            column == 2)
        {
            column--;
        }

        updateRows();
        return;
    }

    row--;

    if (getActivePreset()->getBindingByIndex(row + rowOffset).isNote() &&
        column == 2)
    {
        column--;
    }

    updateRows();
}

void VmpcMidiScreen::openWindow()
{
    openScreenById(ScreenId::VmpcMidiPresetsScreen);
}

void VmpcMidiScreen::acceptLearnCandidate()
{
    if (!learnCandidate)
    {
        return;
    }

    auto &binding = getActivePreset()->getBindingByIndex(row + rowOffset);

    binding.setMessageType(learnCandidate->getMessageType());

    binding.setMidiNumber(learnCandidate->getMidiNumber());

    if (learnCandidate->isController())
    {
        binding.setMidiValue(learnCandidate->getMidiValue());
    }

    binding.setMidiChannelIndex(learnCandidate->getMidiChannelIndex());
}

void VmpcMidiScreen::down()
{
    if (learning)
    {
        acceptLearnCandidate();
        learnCandidate.reset();
    }

    auto preset = getActivePreset();

    if (row == 4)
    {
        if (rowOffset + 5 >= preset->getBindings().size())
        {
            return;
        }

        rowOffset++;

        if (preset->getBindingByIndex(row + rowOffset).isNote() && column == 2)
        {
            column--;
        }

        updateRows();
        return;
    }

    row++;

    if (preset->getBindingByIndex(row + rowOffset).isNote() && column == 2)
    {
        column--;
    }

    updateRows();
}

void VmpcMidiScreen::left()
{
    if (column == 0)
    {
        return;
    }

    column--;

    if (getActivePreset()->getBindingByIndex(row + rowOffset).isNote() &&
        column == 2)
    {
        column--;
    }

    updateRows();
}

void VmpcMidiScreen::right()
{
    if (column == 3)
    {
        return;
    }

    column++;

    if (getActivePreset()->getBindingByIndex(row + rowOffset).isNote() &&
        column == 2)
    {
        column++;
    }

    updateRows();
}

void VmpcMidiScreen::setLearning(bool b)
{
    learning = b;

    if (!learning)
    {
        learnCandidate.reset();
    }
    else
    {
        learnCandidate = getActivePreset()->getBindingByIndex(row + rowOffset);
    }

    findChild<TextComp>("fk2")->setBlinking(learning);
    findChild<TextComp>("fk3")->setBlinking(learning);
    ls.lock()->setFunctionKeysArrangement(learning ? 1 : 0);
}

bool VmpcMidiScreen::hasMappingChanged()
{
    if (getActivePreset()->getBindings().size() !=
        uneditedActivePresetCopy->getBindings().size())
    {
        return true;
    }

    for (int i = 0; i < getActivePreset()->getBindings().size(); i++)
    {
        if (getActivePreset()->getBindingByIndex(i) !=
            uneditedActivePresetCopy->getBindingByIndex(i))
        {
            return true;
        }
    }

    return false;
}

void VmpcMidiScreen::function(int i)
{
    switch (i)
    {
        case 0:
            if (learning)
            {
                return;
            }

            if (hasMappingChanged())
            {
                const auto screen =
                    mpc.screens
                        ->get<ScreenId::VmpcDiscardMappingChangesScreen>();
                screen->setNextScreen("vmpc-settings");
                openScreenById(ScreenId::VmpcDiscardMappingChangesScreen);
                return;
            }

            openScreenById(ScreenId::VmpcSettingsScreen);
            break;
        case 1:
            if (learning)
            {
                return;
            }

            if (hasMappingChanged())
            {
                const auto screen =
                    mpc.screens
                        ->get<ScreenId::VmpcDiscardMappingChangesScreen>();
                screen->setNextScreen("vmpc-keyboard");
                openScreenById(ScreenId::VmpcDiscardMappingChangesScreen);
                return;
            }

            openScreenById(ScreenId::VmpcKeyboardScreen);
            break;
        case 2:
            if (learning)
            {
                setLearning(false);
                learnCandidate.reset();
                updateRows();
                return;
            }

            if (hasMappingChanged())
            {
                const auto screen =
                    mpc.screens
                        ->get<ScreenId::VmpcDiscardMappingChangesScreen>();
                screen->setNextScreen("vmpc-auto-save");
                openScreenById(ScreenId::VmpcDiscardMappingChangesScreen);
                return;
            }

            openScreenById(ScreenId::VmpcAutoSaveScreen);
            break;
        case 3:
            if (learning)
            {
                acceptLearnCandidate();
            }

            setLearning(!learning);
            learnCandidate.reset();

            updateRows();
            break;
        case 5:
        {
            if (learning)
            {
                return;
            }

            std::string popupMsg;

            if (hasMappingChanged())
            {
                const auto path =
                    mpc.paths->getDocuments()->activeMidiControlPresetPath();

                json presetJson;

                to_json(presetJson, *getActivePreset());

                set_file_data(path, presetJson.dump(4));

                popupMsg = "MIDI mapping saved";
            }
            else
            {
                popupMsg = "MIDI mapping unchanged";
            }

            ls.lock()->showPopupForMs(popupMsg, 1000);
            break;
        }
    }
}

void VmpcMidiScreen::setLearnCandidate(const bool isNote,
                                       const int8_t channelIndex,
                                       const int8_t number, const int8_t value)
{
    learnCandidate->setMessageType(isNote ? BindingMessageType::Note
                                          : BindingMessageType::Controller);
    learnCandidate->setMidiChannelIndex(channelIndex);
    learnCandidate->setMidiNumber(number);

    if (!isNote)
    {
        learnCandidate->setMidiValue(value);
    }

    updateRows();
}

bool VmpcMidiScreen::isLearning()
{
    return learning;
}

void VmpcMidiScreen::updateRows()
{
    for (int i = 0; i < 5; i++)
    {
        const auto typeLabel = findChild<Label>("type" + std::to_string(i));
        const auto typeField = findChild<Field>("type" + std::to_string(i));

        constexpr int length = 15;

        const auto targetText =
            StrUtil::padRight(getActivePreset()
                                  ->getBindingByIndex(i + rowOffset)
                                  .getTargetDisplayName(),
                              " ", length) +
            ":";

        typeLabel->setText(targetText);

        auto &binding =
            learning && row == i && learnCandidate.has_value()
                ? learnCandidate.value()
                : getActivePreset()->getBindingByIndex(i + rowOffset);

        std::string type = messageTypeToString(binding.getMessageType());

        typeField->setText(type);
        typeField->setInverted(row == i && column == 0);

        const auto channelField =
            findChild<Field>("channel" + std::to_string(i));

        if (binding.getMidiChannelIndex() == -1)
        {
            channelField->setText("all");
        }
        else
        {
            channelField->setText(
                std::to_string(binding.getMidiChannelIndex() + 1));
        }

        channelField->setInverted(row == i && column == 3);

        const auto numberField = findChild<Field>("number" + std::to_string(i));

        if (binding.isEnabled())
        {
            numberField->setTextPadded(binding.getMidiNumber());
        }
        else
        {
            numberField->setText("OFF");
        }

        numberField->setInverted(row == i && column == 1);

        const auto valueLabel = findChild<Label>("value" + std::to_string(i));
        const auto valueField = findChild<Field>("value" + std::to_string(i));

        if (binding.isNote())
        {
            valueLabel->Hide(true);
            valueField->Hide(true);
        }
        else
        {
            if (binding.getMidiValue() == -1)
            {
                valueField->setText("all");
            }
            else
            {
                valueField->setTextPadded(binding.getMidiValue());
            }

            valueLabel->Hide(false);
            valueField->Hide(false);
        }

        valueField->setInverted(row == i && column == 2);

        typeField->setBlinking(learning && i == row);
        channelField->setBlinking(learning && i == row);
        numberField->setBlinking(learning && i == row);
        valueField->setBlinking(learning && i == row);
    }

    displayUpAndDown();
}

void VmpcMidiScreen::displayUpAndDown()
{
    findChild<Label>("up")->Hide(rowOffset == 0);
    findChild<Label>("down")->Hide(rowOffset + 5 >=
                                   getActivePreset()->getBindings().size());
}

std::shared_ptr<MidiControlPresetV3> VmpcMidiScreen::getActivePreset()
{
    return mpc.clientEventController->getClientMidiEventController()
        ->getExtendedController()
        ->getActivePreset();
}
