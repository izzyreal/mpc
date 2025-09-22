#include "Hardware.hpp"

#include "HwComponent.hpp"
#include "TopPanel.hpp"
#include "Button.hpp"
#include "DataWheel.hpp"
#include "HwPad.hpp"
#include "Led.hpp"
#include "Pot.hpp"
#include "HwSlider.hpp"

#include "PadAndButtonKeyboard.hpp"

#include <Mpc.hpp>

using namespace mpc::hardware;

Hardware::Hardware(mpc::Mpc& mpc)
{
    padAndButtonKeyboard = new PadAndButtonKeyboard(mpc);

    topPanel = std::make_shared<TopPanel>();
    
    buttonLabels = { "left", "right", "up", "down",	"rec", "overdub", "stop", "play", "play-start", "main-screen", "prev-step-event", "next-step-event", "go-to", "prev-bar-start", "next-bar-end", "tap", "next-seq", "track-mute", "open-window", "full-level", "sixteen-levels", "f1", "f2", "f3", "f4", "f5", "f6", "shift", "shift_#1", "shift_#2", "shift_#3", "enter", "undo-seq", "erase", "after", "bank-a", "bank-b", "bank-c", "bank-d", "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "0_extra", "1_extra", "2_extra", "3_extra", "4_extra", "5_extra", "6_extra", "7_extra", "8_extra", "9_extra" };
    
    for (auto& l : buttonLabels)
    {
        buttons.push_back(std::make_shared<Button>(mpc, l));
        components.push_back(buttons.back());
    }
    
    for (int i = 0; i <= 9; i++)
    {
        buttons.push_back(std::make_shared<Button>(mpc, std::to_string(i)));
        components.push_back(buttons.back());
    }
    
    for (int i = 0; i <= 15; i++)
    {
        pads.push_back(std::make_shared<HwPad>(mpc, i));
        components.push_back(pads.back());
    }
    
    dataWheel = std::make_shared<DataWheel>(mpc);
    
    std::vector<std::string> ledLabels{ "full-level", "sixteen-levels", "next-seq", "track-mute", "pad-bank-a", "pad-bank-b", "pad-bank-c", "pad-bank-d", "after", "undo-seq", "rec", "overdub", "play" };
    
    for (auto& l : ledLabels)
        leds.push_back(std::make_shared<Led>(l));
    
    recPot = std::make_shared<Pot>(mpc, "rec");
    volPot = std::make_shared<Pot>(mpc, "vol");
    
    slider = std::make_shared<Slider>(mpc);
}

std::vector<std::string>& Hardware::getButtonLabels()
{
    return buttonLabels;
}

std::shared_ptr<TopPanel> Hardware::getTopPanel()
{
    return topPanel;
}

std::shared_ptr<Pot> Hardware::getRecPot()
{
    return recPot;
}

std::shared_ptr<Pot> Hardware::getVolPot()
{
    return volPot;
}

std::shared_ptr<HwPad> Hardware::getPad(int index)
{
    return pads[index];
}

std::vector<std::shared_ptr<HwPad>>& Hardware::getPads()
{
    return pads;
}

std::shared_ptr<Button> Hardware::getButton(std::string label)
{
    for (auto b : buttons)
        if (b->getLabel() == label) return b;
    return {};
}

std::shared_ptr<Led> Hardware::getLed(std::string label)
{
    for (auto& l : leds)
    {
        if (l->getLabel() == label)
            return l;
    }
    return {};
}

std::vector<std::shared_ptr<Led>> Hardware::getLeds() {
    return leds;
}

std::shared_ptr<DataWheel> Hardware::getDataWheel() {
    return dataWheel;
}

std::shared_ptr<Slider> Hardware::getSlider() {
    return slider;
}

std::shared_ptr<HwComponent> Hardware::getComponentByLabel(const std::string& label)
{
    for (auto& c : components)
    {
        if (c->getLabel() == label)
        {
            return c;
        }
    }
    
    return {};
}

PadAndButtonKeyboard* Hardware::getPadAndButtonKeyboard()
{
    return padAndButtonKeyboard;
}
