#include "VmpcKeyboardScreen.hpp"

#include <Mpc.hpp>

#include <hardware/Hardware.hpp>
#include <hardware/HwPad.hpp>

#include <lcdgui/screens/dialog2/PopupScreen.hpp>
#include <lcdgui/screens/window/VmpcDiscardMappingChangesScreen.hpp>

#include <lcdgui/Parameter.hpp>
#include <lcdgui/Label.hpp>
#include <lcdgui/Field.hpp>

#include <controls/KbMapping.hpp>

#include <StrUtil.hpp>

#include <controls/KeyCodes.hpp>

using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui::screens::dialog2;
using namespace mpc::lcdgui;
using namespace mpc::controls;

VmpcKeyboardScreen::VmpcKeyboardScreen(mpc::Mpc& mpc, int layerIndex)
: ScreenComponent(mpc, "vmpc-keyboard", layerIndex)
{
    for (int i = 0; i < 5; i++)
    {
        auto param = std::make_shared<Parameter>(mpc, "                ", "row" + std::to_string(i), 2, 3 + (i * 9), 17 * 6);
        addChild(param);
    }
}

void VmpcKeyboardScreen::turnWheel(int i)
{
    init();
    auto kbMapping = mpc.getControls()->getKbMapping().lock();
    auto label = kbMapping->getLabelKeyMap()[row + rowOffset].first;
    auto oldKeyCode = kbMapping->getKeyCodeFromLabel(label);
    auto newKeyCode = i > 0 ? KbMapping::getNextKeyCode(oldKeyCode) : KbMapping::getPreviousKeyCode(oldKeyCode);

    kbMapping->setKeyCodeForLabel(newKeyCode, label);
    updateRows();
}

void VmpcKeyboardScreen::open()
{
    auto screen = mpc.screens->get<VmpcDiscardMappingChangesScreen>("vmpc-discard-mapping-changes");
    screen->saveAndLeave = [this](){this->mpc.getControls()->getKbMapping().lock()->exportMapping();};
    screen->discardAndLeave = [this](){this->mpc.getControls()->getKbMapping().lock()->importMapping();};
    screen->stayScreen = "vmpc-keyboard";

    findChild<Label>("up")->setText("\u00C7");
    findChild<Label>("down")->setText("\u00C6");
    
    setLearning(false);
    setLearnCandidate(-1);
    updateRows();
}

void VmpcKeyboardScreen::up()
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

void VmpcKeyboardScreen::down()
{
    if (learning)
        return;

    auto kbMapping = mpc.getControls()->getKbMapping().lock();

    if (row == 4)
    {
        if (rowOffset + 5 >= kbMapping->getLabelKeyMap().size())
            return;
        
        rowOffset++;
        updateRows();
        return;
    }
    
    row++;
    updateRows();
}

void VmpcKeyboardScreen::setLearning(bool b)
{
    learning = b;
    findChild<TextComp>("fk2")->setBlinking(learning);
    findChild<TextComp>("fk3")->setBlinking(learning);
    ls->setFunctionKeysArrangement(learning ? 1 : 0);
}

bool VmpcKeyboardScreen::hasMappingChanged()
{
    auto persisted = KbMapping();
    auto inMem = mpc.getControls()->getKbMapping().lock();

    for (auto& mapping : inMem->getLabelKeyMap())
    {
        if (inMem->getKeyCodeFromLabel(mapping.first) != persisted.getKeyCodeFromLabel(mapping.first))
            return true;
    }

    for (auto& mapping : persisted.getLabelKeyMap())
    {
        if (inMem->getKeyCodeFromLabel(mapping.first) != persisted.getKeyCodeFromLabel(mapping.first))
            return true;
    }

    return false;
}

void VmpcKeyboardScreen::function(int i)
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
                auto kbMapping = mpc.getControls()->getKbMapping().lock();
                auto mapping = kbMapping->getLabelKeyMap()[row + rowOffset];
                auto oldKeyCode = mapping.second;
                
                if (learnCandidate != oldKeyCode)
                {
                    kbMapping->setKeyCodeForLabel(learnCandidate, mapping.first);
                }
            }
            
            setLearning(!learning);
            
            setLearnCandidate(-1);
            updateRows();
            break;
        case 4:
            if (learning)
                return;
            
            openScreen("vmpc-reset-keyboard");
            break;
        case 5:
            if (learning)
                return;

            auto popupScreen = mpc.screens->get<PopupScreen>("popup");
            openScreen("popup");

            if (hasMappingChanged())
            {
                mpc.getControls()->getKbMapping().lock()->exportMapping();
                popupScreen->setText("Keyboard mapping saved");
            }
            else
            {
                popupScreen->setText("Keyboard mapping unchanged");
            }

            popupScreen->returnToScreenAfterMilliSeconds("vmpc-keyboard", 1000);

            break;
    }
}

void VmpcKeyboardScreen::mainScreen()
{
    if (hasMappingChanged())
    {
        openScreen("vmpc-discard-mapping-changes");
        return;
    }
    
    ScreenComponent::mainScreen();
}

void VmpcKeyboardScreen::setLearnCandidate(const int rawKeyCode)
{
    learnCandidate = rawKeyCode;
    updateRows();
}

bool VmpcKeyboardScreen::isLearning()
{
    return learning;
}

void VmpcKeyboardScreen::updateRows()
{
    auto kbMapping = mpc.getControls()->getKbMapping().lock();
    auto& labelKeyMap = kbMapping->getLabelKeyMap();

    for (int i = 0; i < 5; i++)
    {
        auto l = findChild<Label>("row" + std::to_string(i));
        auto f = findChild<Field>("row" + std::to_string(i));
        
        int length = 15;
        auto mapping = labelKeyMap[i + rowOffset];
        auto labelText = StrUtil::padRight(mapping.first, " ", length) + ":";

        for (auto& c : labelText)
        {
            if (c == '_' || c == '-') c = ' ';
        }

        l->setText(labelText);
        f->setText(kbMapping->getKeyCodeString(mapping.second));
        f->setInverted(row == i);
        
        if (learning && i == row)
        {
            f->setText(KeyCodes::getKeyCodeName(learnCandidate));
            f->setBlinking(true);
        }
        else
        {
            f->setBlinking(false);
        }
    }
    
    displayUpAndDown();
}

void VmpcKeyboardScreen::displayUpAndDown()
{
    auto labelKeyMapSize = mpc.getControls()->getKbMapping().lock()->getLabelKeyMap().size();
    findChild<Label>("up")->Hide(rowOffset == 0);
    findChild<Label>("down")->Hide(rowOffset + 5 >= labelKeyMapSize);
}
