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

    Command& cmd = labelCommands[row + rowOffset].second;

    if (column == 0)
    {
        cmd.isNote = i > 0;
    }
    else if (column == 1)
    {
        cmd.channelIndex += i;
        if (cmd.channelIndex < -1)
        {
            cmd.channelIndex = -1;
        }
        else if (cmd.channelIndex > 15)
        {
            cmd.channelIndex = 15;
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
    findChild<Label>("up").lock()->setText("\u00C7");
    findChild<Label>("down").lock()->setText("\u00C6");
    
    setLearning(false);
    setLearnCandidate(-1);
    updateRows();
}

void VmpcMidiScreen::up()
{
    if (learning)
        return;
    
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

void VmpcMidiScreen::down()
{
    if (learning)
        return;
    
    if (row == 4)
    {
        if (rowOffset + 5 >= labelCommands.size())
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
    findChild<TextComp>("fk2").lock()->setBlinking(learning);
    findChild<TextComp>("fk3").lock()->setBlinking(learning);
    ls.lock()->setFunctionKeysArrangement(learning ? 1 : 0);
}

bool VmpcMidiScreen::hasMappingChanged()
{
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
                return;
            if (hasMappingChanged())
            {
                auto screen = mpc.screens->get<VmpcDiscardMappingChangesScreen>("vmpc-discard-mapping-changes");
                screen->nextScreen = "vmpc-keyboard";
                openScreen("vmpc-discard-mapping-changes");
            }
            openScreen("vmpc-keyboard");
            break;
        case 2:
            if (learning)
            {
                setLearning(false);
                setLearnCandidate(-1);
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
                auto label = labelCommands[row + rowOffset].first;
                auto oldMidiCommand = labelCommands[row + rowOffset].second;

//                if (learnCandidate != oldMidiCommand)
//                {
//                    //
//                }
            }
            
            setLearning(!learning);
            
            setLearnCandidate(-1);
            updateRows();
            break;
        case 4:
            if (learning)
                return;

            // TODO Implement vmpc-reset-midi screen
            openScreen("vmpc-reset-midi");
            break;
        case 5:
            if (learning)
                return;

            auto popupScreen = mpc.screens->get<PopupScreen>("popup");
            openScreen("popup");

            if (hasMappingChanged())
            {
                // TODO Persist mapping
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

void VmpcMidiScreen::mainScreen()
{
    if (hasMappingChanged())
    {
        openScreen("vmpc-discard-mapping-changes");
        return;
    }
    
    ScreenComponent::mainScreen();
}

void VmpcMidiScreen::setLearnCandidate(const int rawKeyCode)
{
    learnCandidate = rawKeyCode;
    updateRows();
}

void VmpcMidiScreen::setLabelCommand(std::string& label, Command& c)
{
    for (auto& labelCommand : labelCommands)
    {
        auto label2 = labelCommand.first;

        if (label == label2)
        {
            labelCommand.second = c;
            return;
        }
    }

    labelCommands.push_back({label, c});
}

bool VmpcMidiScreen::isLearning()
{
    return learning;
}

void VmpcMidiScreen::updateRows()
{
    for (int i = 0; i < 5; i++)
    {
        auto typeLabel = findChild<Label>("type" + std::to_string(i)).lock();
        auto typeField = findChild<Field>("type" + std::to_string(i)).lock();
        
        int length = 15;
        
        auto labelText = StrUtil::padRight(labelCommands[i + rowOffset].first, " ", length) + ":";
        
        typeLabel->setText(labelText);
        auto cmd = labelCommands[i + rowOffset].second;
        std::string type = cmd.isNote ? "Note" : "CC";
        typeField->setText(type);
        typeField->setInverted(row == i && column == 0);

        auto channelField = findChild<Field>("channel" + std::to_string(i)).lock();

        if (cmd.channelIndex == -1)
        {
            channelField->setText("all");
        }
        else
        {
            channelField->setText("ch " + std::to_string(cmd.channelIndex + 1));
        }

        channelField->setInverted(row == i && column == 1);

        auto valueField = findChild<Field>("value" + std::to_string(i)).lock();

        if (cmd.value == -1)
        {
            valueField->setText("OFF");
        }
        else
        {
            valueField->setTextPadded(cmd.value);
        }

        valueField->setInverted(row == i && column == 2);

        if (learning && i == row)
        {
            // TODO Display new MIDI command description
            typeField->setBlinking(true);
        }
        else
        {
            typeField->setBlinking(false);
        }
    }
    
    displayUpAndDown();
}

void VmpcMidiScreen::displayUpAndDown()
{
    findChild<Label>("up").lock()->Hide(rowOffset == 0);
    findChild<Label>("down").lock()->Hide(rowOffset + 5 >= labelCommands.size());
}
