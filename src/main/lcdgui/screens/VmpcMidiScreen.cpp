#include "VmpcMidiScreen.hpp"

#include <Mpc.hpp>

#include "lcdgui/screens/window/VmpcDiscardMappingChangesScreen.hpp"

#include "lcdgui/Parameter.hpp"
#include "lcdgui/Label.hpp"
#include "lcdgui/Field.hpp"

#include <StrUtil.hpp>

using namespace mpc::nvram;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui::screens::dialog2;
using namespace mpc::lcdgui;

VmpcMidiScreen::VmpcMidiScreen(mpc::Mpc &mpc, int layerIndex)
    : ScreenComponent(mpc, "vmpc-midi", layerIndex)
{
    for (int i = 0; i < 5; i++)
    {
        auto typeParam = std::make_shared<Parameter>(mpc, "                ", "type" + std::to_string(i), 2, 3 + (i * 9), 4 * 6);
        addChild(typeParam);

        auto numberParam = std::make_shared<Parameter>(mpc, "#:", "number" + std::to_string(i), 125, 3 + (i * 9), 3 * 6);
        addChild(numberParam);

        auto valueParam = std::make_shared<Parameter>(mpc, "v:", "value" + std::to_string(i), 164, 3 + (i * 9), 3 * 6);
        addChild(valueParam);

        auto channelParam = std::make_shared<Parameter>(mpc, "ch:", "channel" + std::to_string(i), 202, 3 + (i * 9), 3 * 6);
        addChild(channelParam);
    }
}

void VmpcMidiScreen::turnWheel(int i)
{

    MidiControlCommand &cmd = activePreset->rows[row + rowOffset];

    if (column == 0)
    {
        cmd.setMidiMessageType(i > 0 ? MidiControlCommand::MidiMessageType::NOTE : MidiControlCommand::MidiMessageType::CC);
    }
    else if (column == 1)
    {
        cmd.setNumber(std::clamp(cmd.getNumber() + i, -1, 127));
    }
    else if (column == 2)
    {
        cmd.setValue(std::clamp(cmd.getValue() + i, -1, 127));
    }
    else if (column == 3)
    {
        cmd.setMidiChannelIndex(std::clamp(cmd.getMidiChannelIndex() + i, -1, 15));
    }

    updateRows();
}

void VmpcMidiScreen::open()
{
    auto screen = mpc.screens->get<VmpcDiscardMappingChangesScreen>();

    screen->discardAndLeave = [this]()
    {
        activePreset = std::make_shared<MidiControlPreset>();

        for (auto &presetRow : uneditedActivePresetCopy->rows)
        {
            activePreset->rows.emplace_back(presetRow);
        }

        uneditedActivePresetCopy = std::make_shared<MidiControlPreset>();
    };

    screen->saveAndLeave = [this]()
    {
        uneditedActivePresetCopy = std::make_shared<MidiControlPreset>();
    };

    screen->stayScreen = "vmpc-midi";

    if (ls->isPreviousScreenNot<VmpcDiscardMappingChangesScreen>())
    {
        uneditedActivePresetCopy = std::make_shared<MidiControlPreset>();

        for (auto &presetRow : activePreset->rows)
        {
            uneditedActivePresetCopy->rows.emplace_back(presetRow);
        }
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

        if (activePreset->rows[row + rowOffset].isNote() &&
            column == 2)
        {
            column--;
        }

        updateRows();
        return;
    }

    row--;

    if (activePreset->rows[row + rowOffset].isNote() &&
        column == 2)
    {
        column--;
    }

    updateRows();
}

void VmpcMidiScreen::openWindow()
{
    mpc.getLayeredScreen()->openScreen<VmpcMidiPresetsScreen>();
}

void VmpcMidiScreen::acceptLearnCandidate()
{
    if (learnCandidate.isEmpty())
    {
        return;
    }

    activePreset->rows[row + rowOffset].setMidiMessageType(learnCandidate.getMidiMessageType());
    activePreset->rows[row + rowOffset].setNumber(learnCandidate.getNumber());

    if (learnCandidate.isCC())
    {
        activePreset->rows[row + rowOffset].setValue(learnCandidate.getValue());
    }

    activePreset->rows[row + rowOffset].setMidiChannelIndex(learnCandidate.getMidiChannelIndex());
}

void VmpcMidiScreen::down()
{
    if (learning)
    {
        acceptLearnCandidate();
        learnCandidate.reset();
    }

    if (row == 4)
    {
        if (rowOffset + 5 >= activePreset->rows.size())
        {
            return;
        }

        rowOffset++;

        if (activePreset->rows[row + rowOffset].isNote() &&
            column == 2)
        {
            column--;
        }

        updateRows();
        return;
    }

    row++;

    if (activePreset->rows[row + rowOffset].isNote() &&
        column == 2)
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

    if (activePreset->rows[row + rowOffset].isNote() &&
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

    if (activePreset->rows[row + rowOffset].isNote() &&
        column == 2)
    {
        column++;
    }

    updateRows();
}

void VmpcMidiScreen::setLearning(bool b)
{
    learning = b;
    findChild<TextComp>("fk2")->setBlinking(learning);
    findChild<TextComp>("fk3")->setBlinking(learning);
    ls->setFunctionKeysArrangement(learning ? 1 : 0);
}

bool VmpcMidiScreen::hasMappingChanged()
{
    if (activePreset->rows.size() != uneditedActivePresetCopy->rows.size())
    {
        return true;
    }

    for (int i = 0; i < activePreset->rows.size(); i++)
    {
        if (!activePreset->rows[i].equals(uneditedActivePresetCopy->rows[i]))
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
                auto screen = mpc.screens->get<VmpcDiscardMappingChangesScreen>();
                screen->nextScreen = "vmpc-settings";
                mpc.getLayeredScreen()->openScreen<VmpcDiscardMappingChangesScreen>();
                return;
            }

            mpc.getLayeredScreen()->openScreen<VmpcSettingsScreen>();
            break;
        case 1:
            if (learning)
            {
                return;
            }

            if (hasMappingChanged())
            {
                auto screen = mpc.screens->get<VmpcDiscardMappingChangesScreen>();
                screen->nextScreen = "vmpc-keyboard";
                mpc.getLayeredScreen()->openScreen<VmpcDiscardMappingChangesScreen>();
                return;
            }

            mpc.getLayeredScreen()->openScreen<VmpcKeyboardScreen>();
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
                auto screen = mpc.screens->get<VmpcDiscardMappingChangesScreen>();
                screen->nextScreen = "vmpc-auto-save";
                mpc.getLayeredScreen()->openScreen<VmpcDiscardMappingChangesScreen>();
                return;
            }

            mpc.getLayeredScreen()->openScreen<VmpcAutoSaveScreen>();
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
                MidiControlPersistence::saveCurrentState(mpc);
                popupMsg = "MIDI mapping saved";
            }
            else
            {
                popupMsg = "MIDI mapping unchanged";
            }

            ls->showPopupForMs(popupMsg, 1000);
            break;
        }
    }
}

void VmpcMidiScreen::setLearnCandidate(const bool isNote, const int8_t channelIndex, const int8_t number, const int8_t value)
{
    learnCandidate.setMidiMessageType(isNote ? MidiControlCommand::MidiMessageType::NOTE : MidiControlCommand::MidiMessageType::CC);
    learnCandidate.setMidiChannelIndex(channelIndex);
    learnCandidate.setNumber(number);

    if (!isNote)
    {
        learnCandidate.setValue(value);
    }

    updateRows();
}

void VmpcMidiScreen::updateOrAddActivePresetCommand(MidiControlCommand &c)
{
    for (auto &labelCommand : activePreset->rows)
    {
        if (c.getMpcHardwareLabel() == labelCommand.getMpcHardwareLabel())
        {
            labelCommand = c;
            return;
        }
    }

    activePreset->rows.emplace_back(c);
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

        int length = 15;

        const auto labelText = StrUtil::padRight(activePreset->rows[i + rowOffset].getMpcHardwareLabel(), " ", length) + ":";

        typeLabel->setText(labelText);
        MidiControlCommand &cmd =
            (learning && row == i && !learnCandidate.isEmpty()) ? learnCandidate : activePreset->rows[i + rowOffset];

        std::string type = cmd.isNote() ? "Note" : "CC";

        typeField->setText(type);
        typeField->setInverted(row == i && column == 0);

        auto channelField = findChild<Field>("channel" + std::to_string(i));

        if (cmd.getMidiChannelIndex() == -1)
        {
            channelField->setText("all");
        }
        else
        {
            channelField->setText(std::to_string(cmd.getMidiChannelIndex() + 1));
        }

        channelField->setInverted(row == i && column == 3);

        auto numberField = findChild<Field>("number" + std::to_string(i));

        if (cmd.getNumber() == -1)
        {
            numberField->setText("OFF");
        }
        else
        {
            numberField->setTextPadded(cmd.getNumber());
        }

        numberField->setInverted(row == i && column == 1);

        auto valueLabel = findChild<Label>("value" + std::to_string(i));
        auto valueField = findChild<Field>("value" + std::to_string(i));

        if (cmd.isNote())
        {
            valueLabel->Hide(true);
            valueField->Hide(true);
        }
        else
        {
            if (cmd.getValue() == -1)
            {
                valueField->setText("all");
            }
            else
            {
                valueField->setTextPadded(cmd.getValue());
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
    findChild<Label>("down")->Hide(rowOffset + 5 >= activePreset->rows.size());
}

std::shared_ptr<MidiControlPreset> VmpcMidiScreen::getActivePreset()
{
    return activePreset;
}
