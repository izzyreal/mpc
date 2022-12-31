#include "VmpcMidiScreen.hpp"

#include <Mpc.hpp>

#include <hardware/Hardware.hpp>
#include <hardware/HwPad.hpp>

#include <lcdgui/screens/dialog2/PopupScreen.hpp>
#include <lcdgui/screens/window/VmpcDiscardMappingChangesScreen.hpp>

#include <lcdgui/Parameter.hpp>
#include <lcdgui/Label.hpp>
#include <lcdgui/Field.hpp>

#include <lang/StrUtil.hpp>
#include <Logger.hpp>

using namespace mpc::nvram;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui::screens::dialog2;
using namespace mpc::lcdgui;
using namespace mpc::controls;
using namespace moduru::lang;

VmpcMidiScreen::VmpcMidiScreen(mpc::Mpc& mpc, int layerIndex)
: ScreenComponent(mpc, "vmpc-midi", layerIndex)
{
    for (int i = 0; i < 5; i++)
    {
        auto typeParam = std::make_shared<Parameter>(mpc, "                ", "type" + std::to_string(i), 2, 3 + (i * 9), 4 * 6);
        addChild(typeParam);

        auto channelParam = std::make_shared<Parameter>(mpc, "", "channel" + std::to_string(i), 142, 3 + (i * 9), 5 * 6);
        addChild(channelParam);

        auto valueParam = std::make_shared<Parameter>(mpc, "", "value" + std::to_string(i), 182, 3 + (i * 9), 3 * 6);
        addChild(valueParam);
    }
}

void VmpcMidiScreen::turnWheel(int i)
{
    init();

    MidiControlCommand& cmd = activePreset->rows[row + rowOffset];

    if (column == 0)
    {
        cmd.isNote = i > 0;
    }
    else if (column == 1)
    {
        cmd.channel += i;
        if (cmd.channel < -1)
        {
            cmd.channel = -1;
        }
        else if (cmd.channel > 15)
        {
            cmd.channel = 15;
        }
    }
    else if (column == 2)
    {
        cmd.value += i;
        if (cmd.value < -1)
        {
            cmd.value = -1;
        }
        else if (cmd.value > 127)
        {
            cmd.value = 127;
        }
    }

    updateRows();
}

void VmpcMidiScreen::open()
{
    auto screen = mpc.screens->get<VmpcDiscardMappingChangesScreen>("vmpc-discard-mapping-changes");

    screen->discardAndLeave = [this](){
        this->activePreset = this->uneditedActivePresetCopy;
        this->uneditedActivePresetCopy = std::make_shared<MidiControlPreset>();
    };

    screen->saveAndLeave = [this](){
        this->uneditedActivePresetCopy = std::make_shared<MidiControlPreset>();
    };

    screen->stayScreen = "vmpc-midi";

    if (ls->getPreviousScreenName() != "vmpc-discard-mapping-changes")
    {
        uneditedActivePresetCopy = activePreset;
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
            return;
        
        rowOffset--;
        updateRows();
        return;
    }
    
    row--;
    updateRows();
}

void VmpcMidiScreen::openWindow()
{
    openScreen("vmpc-midi-presets");
}

void VmpcMidiScreen::acceptLearnCandidate()
{
    if (learnCandidate.isEmpty())
    {
        return;
    }

    activePreset->rows[row + rowOffset] = learnCandidate;
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
            return;
        
        rowOffset++;
        updateRows();
        return;
    }
    
    row++;
    updateRows();
}

void VmpcMidiScreen::left()
{
    if (column == 0) return;
    column--;
    updateRows();
}

void VmpcMidiScreen::right()
{
    if (column == 2) return;
    column++;
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
    switch(i)
    {
        case 0:
            if (learning)
                return;
            
            if (hasMappingChanged())
            {
                auto screen = mpc.screens->get<VmpcDiscardMappingChangesScreen>("vmpc-discard-mapping-changes");
                screen->nextScreen = "vmpc-settings";
                openScreen("vmpc-discard-mapping-changes");
                return;
            }

            openScreen("vmpc-settings");
            break;
        case 1:
            if (learning)
            {
                return;
            }

            if (hasMappingChanged())
            {
                auto screen = mpc.screens->get<VmpcDiscardMappingChangesScreen>("vmpc-discard-mapping-changes");
                screen->nextScreen = "vmpc-keyboard";
                openScreen("vmpc-discard-mapping-changes");
                return;
            }

            openScreen("vmpc-keyboard");
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
                auto screen = mpc.screens->get<VmpcDiscardMappingChangesScreen>("vmpc-discard-mapping-changes");
                screen->nextScreen = "vmpc-auto-save";
                openScreen("vmpc-discard-mapping-changes");
                return;
            }

            openScreen("vmpc-auto-save");
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

            auto popupScreen = mpc.screens->get<PopupScreen>("popup");
            openScreen("popup");

            if (hasMappingChanged())
            {
                MidiControlPersistence::saveCurrentState(mpc);
                popupScreen->setText("MIDI mapping saved");
            }
            else
            {
                popupScreen->setText("MIDI mapping unchanged");
            }

            popupScreen->returnToScreenAfterMilliSeconds("vmpc-midi", 1000);

            break;
        }
    }
}

void VmpcMidiScreen::mainScreen()
{
    if (hasMappingChanged())
    {
        openScreen("vmpc-discard-mapping-changes");
        return;
    }
    
    ScreenComponent::mainScreen();
}

void VmpcMidiScreen::setLearnCandidate(const bool isNote, const char channelIndex, const char value)
{
    learnCandidate.isNote = isNote;
    learnCandidate.channel = channelIndex;
    learnCandidate.value = value;
    updateRows();
}

void VmpcMidiScreen::updateOrAddActivePresetCommand(MidiControlCommand &c)
{
    for (auto& labelCommand : activePreset->rows)
    {
        if (c.label == labelCommand.label)
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
        auto typeLabel = findChild<Label>("type" + std::to_string(i));
        auto typeField = findChild<Field>("type" + std::to_string(i));
        
        int length = 15;
        
        auto labelText = StrUtil::padRight(activePreset->rows[i + rowOffset].label, " ", length) + ":";
        
        typeLabel->setText(labelText);
        MidiControlCommand& cmd =
                (learning && row == i && !learnCandidate.isEmpty()) ?
                learnCandidate : activePreset->rows[i + rowOffset];

        std::string type = cmd.isNote ? "Note" : "CC";

        typeField->setText(type);
        typeField->setInverted(row == i && column == 0);

        auto channelField = findChild<Field>("channel" + std::to_string(i));

        if (cmd.channel == -1)
        {
            channelField->setText("all");
        }
        else
        {
            channelField->setText("ch " + std::to_string(cmd.channel + 1));
        }

        channelField->setInverted(row == i && column == 1);

        auto valueField = findChild<Field>("value" + std::to_string(i));

        if (cmd.value == -1)
        {
            valueField->setText("OFF");
        }
        else
        {
            valueField->setTextPadded(cmd.value);
        }

        valueField->setInverted(row == i && column == 2);

        typeField->setBlinking(learning && i == row);
        channelField->setBlinking(learning && i == row);
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