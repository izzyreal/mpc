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

#include <string>

using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui;
using namespace moduru::lang;
using namespace std;

VmpcKeyboardScreen::VmpcKeyboardScreen(mpc::Mpc& mpc, int layerIndex)
: ScreenComponent(mpc, "vmpc-keyboard", layerIndex)
{
    for (int i = 0; i < 5; i++)
    {
        auto param = make_shared<Parameter>(mpc, "       ", "row" + to_string(i), 2, 3 + (i * 9), 20 * 6);
        
        addChild(param);
    }
}

void VmpcKeyboardScreen::open()
{
    updateRows();
}

void VmpcKeyboardScreen::up()
{
    MLOG("up");
}

void VmpcKeyboardScreen::down()
{
    
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
    
    auto& keyCodeNames = moduru::sys::OsxKeyCodes::keyCodeNames;
    
    auto kbMapping = mpc::controls::KbMapping();
    
    vector<pair<string, string>> padLabelsToKeyCodeNames;
    
    auto hw = mpc.getHardware().lock();
    
    for (auto p : hw->getPads())
    {
        auto label = p.lock()->getLabel();
        padLabelsToKeyCodeNames.push_back({label, keyCodeNames[kbMapping.getKeyCodeFromLabel(label)]});
    }
    
    for (int i = 0; i < 5; i++)
    {
        auto l = findChild<Label>("row" + to_string(i)).lock();
        auto f = findChild<Field>("row" + to_string(i)).lock();
        
        auto labelText = StrUtil::padRight(padLabelsToKeyCodeNames[i].first, " ", 6) + ": ";
        
        l->setText(labelText);
        f->setText(padLabelsToKeyCodeNames[i].second);
        f->setInverted(row == i);
    }
}
