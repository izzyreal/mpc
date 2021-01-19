#include "KbEditor.hpp"

#include <lcdgui/Label.hpp>
#include <controls/KbMapping.hpp>

#include <Logger.hpp>
#include <sys/OsxKeyCodes.hpp>

#include <string>

using namespace mpc::lcdgui::kbeditor;
using namespace mpc::lcdgui;
using namespace std;

KbEditor::KbEditor(mpc::Mpc& mpc)
    : Component("keyboard-editor"), mpc (mpc)
{
    x = 0;
    y = 0;
    w = 400;
    h = 400;
    dirty = true;
}

void KbEditor::Draw(vector<vector<bool>>* pixels)
{
    if (shouldNotDraw(pixels))
    {
        return;
    }

    auto& codes = moduru::sys::OsxKeyCodes::keyCodeNames;
    auto kbMapping = mpc::controls::KbMapping();
    
    mpc::lcdgui::Label* label = nullptr;
    
    
    int counter = 0;
    for (auto const& [keyCode, name] : codes)
    {
        if (counter * 10 > 380) break;
        label = new mpc::lcdgui::Label(mpc, string(name), to_string(keyCode), 0, counter * 10, 200);
        label->SetDirty(true);
        label->Draw(pixels);
        delete label;
        
        label = new mpc::lcdgui::Label(mpc, string(name), string(name), 30, counter * 10, 200);
        label->SetDirty(true);
        label->Draw(pixels);
        delete label;
        
        label = new mpc::lcdgui::Label(mpc, string(name), string(kbMapping.getLabelFromKeyCode(keyCode)), 200, counter * 10, 200);
        label->SetDirty(true);
        label->Draw(pixels);
        delete label;
        
        counter++;
    }

    Component::Draw(pixels);
}
