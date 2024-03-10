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
    
    buttonLabels = { "left", "right", "up", "down",	"rec", "overdub", "stop", "play", "play-start", "main-screen", "prev-step-event", "next-step-event", "go-to", "prev-bar-start", "next-bar-end", "tap", "next-seq", "track-mute", "open-window", "full-level", "sixteen-levels", "f1", "f2", "f3", "f4", "f5", "f6", "shift", "enter", "undo-seq", "erase", "after", "bank-a", "bank-b", "bank-c", "bank-d", "0", "1", "2", "3", "4", "5", "6", "7", "8", "9" };
    
    for (auto& l : buttonLabels)
    {
        buttons.emplace_back(std::make_shared<Button>(mpc, l));
        components.emplace_back(buttons.back());
    }
    
    for (int i = 0; i <= 9; i++)
    {
        buttons.emplace_back(std::make_shared<Button>(mpc, std::to_string(i)));
        components.emplace_back(buttons.back());
    }
    
    for (int i = 0; i <= 15; i++)
    {
        pads.emplace_back(std::make_shared<HwPad>(mpc, i));
        components.emplace_back(pads.back());
    }
    
    dataWheel = std::make_shared<DataWheel>(mpc);

    components.emplace_back(std::make_shared<DataWheelUp>(mpc, *dataWheel));
    components.emplace_back(std::make_shared<DataWheelDown>(mpc, *dataWheel));

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

const std::vector<std::shared_ptr<HwComponent>>& Hardware::getComponents()
{
    return components;
}
