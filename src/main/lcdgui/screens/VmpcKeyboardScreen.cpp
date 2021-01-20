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
using namespace std;

VmpcKeyboardScreen::VmpcKeyboardScreen(mpc::Mpc& mpc, int layerIndex)
: ScreenComponent(mpc, "vmpc-keyboard", layerIndex)
{
    for (int i = 0; i < 5; i++)
    {
        auto param = make_shared<Parameter>(mpc, "                 ", "row" + to_string(i), 2, 3 + (i * 9), 20 * 6);
        
        addChild(param);
    }
    
    updateKeyCodeNames();
}

void VmpcKeyboardScreen::open()
{
    updateRows();
}

void VmpcKeyboardScreen::up()
{
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
            ls.lock()->openScreen("vmpc-settings");
            break;
        case 4:
            // kbMapping.resetToDefault()
            break;
    }
}

void VmpcKeyboardScreen::updateRows()
{
    for (int i = 0; i < 5; i++)
    {
        auto l = findChild<Label>("row" + to_string(i)).lock();
        auto f = findChild<Field>("row" + to_string(i)).lock();
        
        int length = 16;
        
        auto labelText = StrUtil::padRight(labelsToKeyCodeNames[i + rowOffset].first, " ", length) + ": ";
        
        l->setText(labelText);
        f->setText(labelsToKeyCodeNames[i + rowOffset].second);
        f->setInverted(row == i);
    }
}

void VmpcKeyboardScreen::updateKeyCodeNames()
{
    labelsToKeyCodeNames.clear();
    
    auto& keyCodeNames = moduru::sys::OsxKeyCodes::keyCodeNames;
    auto kbMapping = mpc::controls::KbMapping();
    auto hw = mpc.getHardware().lock();
    
    auto pads = hw->getPads();
    auto buttons = hw->getButtons();
    
    vector<weak_ptr<mpc::hardware::HwComponent>> components;
    
    for (auto& p : pads) components.push_back(p);
    for (auto& b : buttons) components.push_back(b);
    
    
    for (auto c : components)
    {
        auto label = c.lock()->getLabel();
        labelsToKeyCodeNames.push_back({label, keyCodeNames[kbMapping.getKeyCodeFromLabel(label)]});
    }
}
