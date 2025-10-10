#include "Hardware.hpp"

#include "TopPanel.hpp"
#include "Led.hpp"

#include "PadAndButtonKeyboard.hpp"

#include <Mpc.hpp>

using namespace mpc::hardware;

Hardware::Hardware(mpc::Mpc& mpc)
{
    padAndButtonKeyboard = new PadAndButtonKeyboard(mpc);

    topPanel = std::make_shared<TopPanel>();
    
    std::vector<std::string> ledLabels{ "full-level", "sixteen-levels", "next-seq", "track-mute", "pad-bank-a", "pad-bank-b", "pad-bank-c", "pad-bank-d", "after", "undo-seq", "rec", "overdub", "play" };
    
    for (auto& l : ledLabels)
        leds.push_back(std::make_shared<Led>(l));
}

std::shared_ptr<TopPanel> Hardware::getTopPanel()
{
    return topPanel;
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

PadAndButtonKeyboard* Hardware::getPadAndButtonKeyboard()
{
    return padAndButtonKeyboard;
}

