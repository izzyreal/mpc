#include "KbEditor.hpp"

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

using namespace mpc::lcdgui::kbeditor;
using namespace mpc::lcdgui;
using namespace moduru::lang;
using namespace std;

KbEditor::KbEditor(mpc::Mpc& mpc)
: Component("keyboard-editor"), mpc (mpc)
{
    x = 0;
    y = 0;
    w = 400;
    h = 400;
        
    for (int i = 0; i < 16; i++)
    {
        auto param = make_shared<Parameter>(mpc, "       ", "row" + to_string(i), 20, 20 + (i * 10), 40 * 6);
        
        addChild(param);
    }
    
    updateRows();
}

void KbEditor::updateRows()
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
    
    for (int i = 0; i < 16; i++)
    {
        auto l = findChild<Label>("row" + to_string(i)).lock();
        auto f = findChild<Field>("row" + to_string(i)).lock();
        
        auto labelText = StrUtil::padRight(padLabelsToKeyCodeNames[i].first, " ", 6) + ": ";
        
        l->setText(labelText);
        f->setText(padLabelsToKeyCodeNames[i].second);
        f->setInverted(row == i);
    }
}

void KbEditor::Draw(vector<vector<bool>>* pixels)
{
    if (shouldNotDraw(pixels))
    {
        return;
    }
    
    Component::Draw(pixels);
}
