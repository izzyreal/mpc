#include "Hardware.hpp"

#include "HwComponent.hpp"
#include "TopPanel.hpp"
#include "Button.hpp"
#include "DataWheel.hpp"
#include "HwPad.hpp"
#include "Led.hpp"
#include "Pot.hpp"
#include "HwSlider.hpp"

#include <Mpc.hpp>

using namespace mpc::hardware;
using namespace std;

Hardware::Hardware(mpc::Mpc& mpc)
{
    topPanel = make_shared<TopPanel>();
    
    buttonLabels = { "left", "right", "up", "down",	"rec", "overdub", "stop", "play", "play-start", "main-screen", "prev-step-event", "next-step-event", "go-to", "prev-bar-start", "next-bar-end", "tap", "next-seq", "track-mute", "open-window", "full-level", "sixteen-levels", "f1", "f2", "f3", "f4", "f5", "f6", "shift", "enter", "undo-seq", "erase", "after", "bank-a", "bank-b", "bank-c", "bank-d", "0", "1", "2", "3", "4", "5", "6", "7", "8", "9" };
    
    for (auto& l : buttonLabels)
    {
        buttons.push_back(make_shared<Button>(mpc, l));
        components.push_back(buttons.back());
    }
    
    for (int i = 0; i <= 9; i++)
    {
        buttons.push_back(make_shared<Button>(mpc, to_string(i)));
        components.push_back(buttons.back());
    }
    
    for (int i = 0; i <= 15; i++)
    {
        pads.push_back(make_shared<HwPad>(mpc, i));
        components.push_back(pads.back());
    }
    
    dataWheel = make_shared<DataWheel>(mpc);
    
    vector<string> ledLabels{ "full-level", "sixteen-levels", "next-seq", "track-mute", "pad-bank-a", "pad-bank-b", "pad-bank-c", "pad-bank-d", "after", "undo-seq", "rec", "overdub", "play" };
    
    for (auto& l : ledLabels)
        leds.push_back(make_shared<Led>(l));
    
    recPot = make_shared<Pot>(mpc, "rec");
    volPot = make_shared<Pot>(mpc, "vol");
    
    slider = make_shared<Slider>(mpc);
}

vector<string>& Hardware::getButtonLabels()
{
    return buttonLabels;
}

weak_ptr<TopPanel> Hardware::getTopPanel()
{
    return topPanel;
}

weak_ptr<Pot> Hardware::getRecPot()
{
    return recPot;
}

weak_ptr<Pot> Hardware::getVolPot()
{
    return volPot;
}

weak_ptr<HwPad> Hardware::getPad(int index)
{
    return pads.at(index);
}

vector<shared_ptr<HwPad>>& Hardware::getPads()
{
    return pads;
}

weak_ptr<Button> Hardware::getButton(string label)
{
    for (auto b : buttons)
        if (b->getLabel().compare(label) == 0) return b;
    return weak_ptr<Button>();
}

vector<weak_ptr<HwComponent>> Hardware::getButtons()
{
    vector<weak_ptr<HwComponent>> result(buttons.size());
    copy(begin(buttons), end(buttons), begin(result));
    return result;
}

weak_ptr<Led> Hardware::getLed(string label)
{
    for (auto l : leds)
    {
        if (l->getLabel().compare(label) == 0)
            return l;
    }
    return weak_ptr<Led>();
}

vector<shared_ptr<Led>> Hardware::getLeds() {
    return leds;
}

weak_ptr<DataWheel> Hardware::getDataWheel() {
    return dataWheel;
}

weak_ptr<Slider> Hardware::getSlider() {
    return slider;
}

weak_ptr<HwComponent> Hardware::getComponentByLabel(const string& label)
{
    weak_ptr<HwComponent> result;
    
    for (auto& c : components)
    {
        if (c.lock()->getLabel().compare(label) == 0)
        {
            result = c;
            break;
        }
    }
    
    return result;
}
