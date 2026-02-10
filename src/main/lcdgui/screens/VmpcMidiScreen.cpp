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

VmpcMidiScreen::VmpcMidiScreen(Mpc &mpc, const int layerIndex)
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

        auto encoderModeParam = std::make_shared<Parameter>(
            mpc, "e:", "encoder-mode" + std::to_string(i), 152, 3 + i * 9,
            3 * 6);
        addChild(encoderModeParam);

        auto channelParam = std::make_shared<Parameter>(
            mpc, "c:", "channel" + std::to_string(i), 190, 3 + i * 9, 3 * 6);
        addChild(channelParam);
    }
}

void VmpcMidiScreen::turnWheel(const int i)
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
        if (binding.isController() && binding.isButtonLike())
        {
            binding.setMidiValue(
                std::clamp(binding.getMidiValue() + i, -1, 127));
        }
        else if (binding.isNonButtonLikeDataWheel())
        {
            binding.setEncoderMode(i > 0
                                       ? BindingEncoderMode::RelativeStateful
                                       : BindingEncoderMode::RelativeStateless);
        }
    }
    else if (column == 3)
    {
        binding.setMidiChannelIndex(
            std::clamp(binding.getMidiChannelIndex() + i, -1, 15));
    }

    displayRows();
}

void VmpcMidiScreen::open()
{
    const auto screen =
        mpc.screens->get<ScreenId::VmpcDiscardMappingChangesScreen>();

    screen->discardAndLeave = [this]
    {
        const auto controller =
            mpc.clientEventController->getClientMidiEventController()
                ->getExtendedController();

        const auto activePreset = controller->getActivePreset();

        activePreset->setBindings(uneditedActivePresetCopy->getBindings());

        uneditedActivePresetCopy = std::make_shared<MidiControlPresetV3>();
    };

    screen->saveAndLeave = [this]
    {
        uneditedActivePresetCopy = std::make_shared<MidiControlPresetV3>();
    };

    screen->stayScreen = "vmpc-midi";

    if (ls.lock()->isPreviousScreenNot(
            {ScreenId::VmpcDiscardMappingChangesScreen}))
    {
        uneditedActivePresetCopy = std::make_shared<MidiControlPresetV3>();

        const auto controller =
            mpc.clientEventController->getClientMidiEventController()
                ->getExtendedController();

        const auto activePreset = controller->getActivePreset();

        uneditedActivePresetCopy->setBindings(activePreset->getBindings());
    }

    findChild<Label>("up")->setText("\u00C7");
    findChild<Label>("down")->setText("\u00C6");

    setLearning(false);
    displayRows();
}

void VmpcMidiScreen::up()
{
    if (isLearning())
    {
        acceptLearnCandidate();
    }

    if (row == 0)
    {
        if (rowOffset == 0)
        {
            return;
        }

        rowOffset--;

        if (isLearning())
        {
            setLearnCandidateToSelectedBinding();
        }

        displayRows();

        if (!isColumn2VisibleAtCurrentRow() && column == 2)
        {
            column--;
            displayRows();
        }

        return;
    }

    row--;

    if (isLearning())
    {
        setLearnCandidateToSelectedBinding();
    }

    displayRows();

    if (!isColumn2VisibleAtCurrentRow() && column == 2)
    {
        column--;
        displayRows();
    }
}

void VmpcMidiScreen::openWindow()
{
    openScreenById(ScreenId::VmpcMidiPresetsScreen);
}

void VmpcMidiScreen::acceptLearnCandidate() const
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
    if (isLearning())
    {
        acceptLearnCandidate();
    }

    const auto preset = getActivePreset();

    if (row == 4)
    {
        if (rowOffset + 5 >= preset->getBindings().size())
        {
            return;
        }

        rowOffset++;

        if (isLearning())
        {
            setLearnCandidateToSelectedBinding();
        }

        displayRows();

        if (!isColumn2VisibleAtCurrentRow() && column == 2)
        {
            column--;
            displayRows();
        }

        return;
    }

    row++;

    if (isLearning())
    {
        setLearnCandidateToSelectedBinding();
    }

    displayRows();

    if (!isColumn2VisibleAtCurrentRow() && column == 2)
    {
        column--;
        displayRows();
    }
}

void VmpcMidiScreen::left()
{
    if (column == 0)
    {
        return;
    }

    column--;

    if (column == 2 && !isColumn2VisibleAtCurrentRow())
    {
        column--;
    }

    displayRows();
}

void VmpcMidiScreen::right()
{
    if (column == 3)
    {
        return;
    }

    column++;

    if (column == 2 && !isColumn2VisibleAtCurrentRow())
    {
        column++;
    }

    displayRows();
}

void VmpcMidiScreen::setLearnCandidateToSelectedBinding()
{
    learnCandidate = getActivePreset()->getBindingByIndex(row + rowOffset);
}

bool VmpcMidiScreen::isColumn2VisibleAtCurrentRow()
{
    return !findChild<Field>("value" + std::to_string(row))->IsHidden() ||
           !findChild<Field>("encoder-mode" + std::to_string(row))->IsHidden();
}

void VmpcMidiScreen::setLearning(const bool b)
{
    learning = b;

    if (!learning)
    {
        learnCandidate.reset();
    }
    else
    {
        setLearnCandidateToSelectedBinding();
    }

    findChild<TextComp>("fk2")->setBlinking(learning);
    findChild<TextComp>("fk3")->setBlinking(learning);
    setFunctionKeysArrangement(learning ? 1 : 0);
}

bool VmpcMidiScreen::hasMappingChanged() const
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

void VmpcMidiScreen::function(const int i)
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
                displayRows();
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
            displayRows();
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
        default:;
    }
}

void VmpcMidiScreen::setLearnCandidate(const bool isNote,
                                       const int8_t channelIndex,
                                       const int8_t number)
{
    if (!isLearning())
    {
        return;
    }

    learnCandidate->setMessageType(isNote ? BindingMessageType::Note
                                          : BindingMessageType::Controller);
    learnCandidate->setMidiChannelIndex(channelIndex);
    learnCandidate->setMidiNumber(number);

    displayRows();
}

bool VmpcMidiScreen::isLearning() const
{
    return learning;
}

void VmpcMidiScreen::displayRows()
{
    for (int i = 0; i < 5; i++)
    {
        const auto typeLabel = findChild<Label>("type" + std::to_string(i));
        const auto typeField = findChild<Field>("type" + std::to_string(i));

        constexpr int length = 15;

        auto &binding =
            learning && row == i && learnCandidate.has_value()
                ? learnCandidate.value()
                : getActivePreset()->getBindingByIndex(i + rowOffset);

        const auto targetText =
            StrUtil::padRight(binding.getTargetDisplayName(), " ", length) +
            ":";

        typeLabel->setText(targetText);

        std::string type = messageTypeToDisplayString(binding.getMessageType());

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
        const auto encoderModeLabel =
            findChild<Label>("encoder-mode" + std::to_string(i));
        const auto encoderModeField =
            findChild<Field>("encoder-mode" + std::to_string(i));

        if (binding.isController() && binding.isButtonLike())
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
            encoderModeLabel->Hide(true);
            encoderModeField->Hide(true);
            valueField->setInverted(row == i && column == 2);
            valueField->setBlinking(learning && i == row);
        }
        else
        {
            valueLabel->Hide(true);
            valueField->Hide(true);

            if (binding.isNonButtonLikeDataWheel())
            {
                encoderModeField->setText(
                    encoderModeToDisplayString(binding.getEncoderMode()));
                encoderModeLabel->Hide(false);
                encoderModeField->Hide(false);
                encoderModeField->setInverted(row == i && column == 2);
                encoderModeField->setBlinking(learning && i == row);
            }
            else
            {
                encoderModeLabel->Hide(true);
                encoderModeField->Hide(true);
            }
        }

        typeField->setBlinking(learning && i == row);
        channelField->setBlinking(learning && i == row);
        numberField->setBlinking(learning && i == row);
    }

    displayUpAndDown();
}

void VmpcMidiScreen::displayUpAndDown()
{
    findChild<Label>("up")->Hide(rowOffset == 0);
    findChild<Label>("down")->Hide(rowOffset + 5 >=
                                   getActivePreset()->getBindings().size());
}

std::shared_ptr<MidiControlPresetV3> VmpcMidiScreen::getActivePreset() const
{
    return mpc.clientEventController->getClientMidiEventController()
        ->getExtendedController()
        ->getActivePreset();
}
