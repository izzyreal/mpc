#include "VmpcKeyboardScreen.hpp"

#include <Mpc.hpp>

#include <hardware/Hardware.hpp>
#include <hardware/HwPad.hpp>

#include <lcdgui/Parameter.hpp>
#include <lcdgui/Label.hpp>
#include <lcdgui/Field.hpp>
#include <controls/KbMapping.hpp>

#include <lang/StrUtil.hpp>
#include <Logger.hpp>
#include <sys/OsxKeyCodes.hpp>

using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui;
using namespace moduru::lang;
using namespace moduru::sys;
using namespace std;

VmpcKeyboardScreen::VmpcKeyboardScreen(mpc::Mpc& mpc, int layerIndex)
: ScreenComponent(mpc, "vmpc-keyboard", layerIndex)
{
    for (int i = 0; i < 5; i++)
    {
        auto param = make_shared<Parameter>(mpc, "                ", "row" + to_string(i), 2, 3 + (i * 9), 17 * 6);
        
        addChild(param);
    }
    
    updateKeyCodeNames();
}

void VmpcKeyboardScreen::open()
{
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
    
    if (row == 4)
    {
        if (rowOffset + 5 >= labelsToKeyCodeNames.size())
            return;
        
        rowOffset++;
        updateRows();
        return;
    }
    
    row++;
    updateRows();
}

void VmpcKeyboardScreen::function(int i)
{
    switch(i)
    {
        case 0:
            if (learning)
                return;
            
            ls.lock()->openScreen("vmpc-settings");
            break;
        case 2:
            if (!learning)
                return;
            
            learning = false;
            findChild<TextComp>("fk2").lock()->setBlinking(false);
            findChild<TextComp>("fk3").lock()->setBlinking(false);
            setLearnCandidate(-1);
            updateRows();
            break;
        case 3:
            learning = !learning;
            ls.lock()->setFunctionKeysArrangement(learning ? 1 : 0);
            findChild<TextComp>("fk2").lock()->setBlinking(learning);
            findChild<TextComp>("fk3").lock()->setBlinking(learning);
            
            if (!learning)
            {
                mpc.getControls().lock()->getKbMapping().lock()->setKeyCodeForLabel(learnCandidate, labelsToKeyCodeNames[row + rowOffset].first);
                updateKeyCodeNames();
            }
            
            setLearnCandidate(-1);
            updateRows();
            break;
        case 4:
            if (learning)
                return;
            
            // kbMapping.resetToDefault()
            break;
    }
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
    for (int i = 0; i < 5; i++)
    {
        auto l = findChild<Label>("row" + to_string(i)).lock();
        auto f = findChild<Field>("row" + to_string(i)).lock();
        
        int length = 15;
        
        auto labelText = StrUtil::padRight(labelsToKeyCodeNames[i + rowOffset].first, " ", length) + ": ";
        
        l->setText(labelText);
        f->setText(labelsToKeyCodeNames[i + rowOffset].second);
        f->setInverted(row == i);
        
        if (learning && i == row)
        {
            f->setText(OsxKeyCodes::keyCodeNames[learnCandidate]);
            f->setBlinking(true);
        }
        else
        {
            f->setBlinking(false);
        }
    }
}

void VmpcKeyboardScreen::updateKeyCodeNames()
{
    labelsToKeyCodeNames.clear();
    
    auto& keyCodeNames = OsxKeyCodes::keyCodeNames;
    auto kbMapping = mpc.getControls().lock()->getKbMapping().lock();
    auto hw = mpc.getHardware().lock();
    
    auto pads = hw->getPads();
    auto buttons = hw->getButtons();
    
    vector<weak_ptr<mpc::hardware::HwComponent>> components;
    
    for (auto& p : pads) components.push_back(p);
    for (auto& b : buttons) components.push_back(b);
    
    
    for (auto c : components)
    {
        auto label = c.lock()->getLabel();
        labelsToKeyCodeNames.push_back({label, keyCodeNames[kbMapping->getKeyCodeFromLabel(label)]});
    }
}
