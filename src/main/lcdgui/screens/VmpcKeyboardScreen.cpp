#include "VmpcKeyboardScreen.hpp"

#include <Mpc.hpp>

#include <hardware/Hardware.hpp>
#include <hardware/HwPad.hpp>

#include <lcdgui/screens/dialog2/PopupScreen.hpp>
#include <lcdgui/Parameter.hpp>
#include <lcdgui/Label.hpp>
#include <lcdgui/Field.hpp>

#include <controls/KbMapping.hpp>

#include <lang/StrUtil.hpp>
#include <Logger.hpp>

#include <sys/KeyCodes.hpp>

using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::dialog2;
using namespace mpc::lcdgui;
using namespace mpc::controls;
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
    setLearning(false);
    setLearnCandidate(-1);
    updateKeyCodeNames();
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

void VmpcKeyboardScreen::setLearning(bool b)
{
    learning = b;
    findChild<TextComp>("fk2").lock()->setBlinking(learning);
    findChild<TextComp>("fk3").lock()->setBlinking(learning);
    ls.lock()->setFunctionKeysArrangement(learning ? 1 : 0);
}

bool VmpcKeyboardScreen::hasMappingChanged()
{
    auto persisted = KbMapping();
    auto inMem = mpc.getControls().lock()->getKbMapping().lock();

    for (auto& label : inMem->getMappedLabels())
    {
        if (inMem->getKeyCodeFromLabel(label) != persisted.getKeyCodeFromLabel(label))
            return true;
    }
    for (auto& label : persisted.getMappedLabels())
    {
        if (inMem->getKeyCodeFromLabel(label) != persisted.getKeyCodeFromLabel(label))
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
            
            mpc.getControls().lock()->getKbMapping().lock()->initializeDefaults();
            updateKeyCodeNames();
            updateRows();
            break;
        case 3:
            if (learning)
            {
                auto label = labelsToKeyCodeNames[row + rowOffset].first;
                auto kbMapping = mpc.getControls().lock()->getKbMapping().lock();
                auto oldKeyCode = kbMapping->getKeyCodeFromLabel(label);
                
                if (learnCandidate != oldKeyCode)
                {
                    kbMapping->setKeyCodeForLabel(learnCandidate, label);
                    updateKeyCodeNames();
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
            mpc.getLayeredScreen().lock()->openScreen("popup");

            if (hasMappingChanged())
            {
                mpc.getControls().lock()->getKbMapping().lock()->exportMapping();
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
            f->setText(KeyCodes::keyCodeNames[learnCandidate]);
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
    
    auto& keyCodeNames = KeyCodes::keyCodeNames;

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

    labelsToKeyCodeNames.push_back({ "datawheel-up", keyCodeNames[kbMapping->getKeyCodeFromLabel("datawheel-up")] });
    labelsToKeyCodeNames.push_back({ "datawheel-down", keyCodeNames[kbMapping->getKeyCodeFromLabel("datawheel-down")] });
}
