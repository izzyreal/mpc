#pragma once

#include <vector>
#include <memory>
#include <string>

namespace mpc { class Mpc; }

namespace mpc::hardware
{
class HwComponent;
class TopPanel;
class Pot;
class Led;
class Slider;
}

namespace mpc::hardware
{

class PadAndButtonKeyboard;

class Hardware final
{

private:
    PadAndButtonKeyboard* padAndButtonKeyboard;
    std::vector<std::shared_ptr<HwComponent>> components;

    std::shared_ptr<TopPanel> topPanel;
    std::vector<std::shared_ptr<Led>> leds;

    std::shared_ptr<Slider> slider;
    std::shared_ptr<Pot> recPot;
    std::shared_ptr<Pot> volPot;

public:
    PadAndButtonKeyboard* getPadAndButtonKeyboard();
    std::shared_ptr<TopPanel> getTopPanel();
    std::shared_ptr<Led> getLed(std::string label);
    std::vector<std::shared_ptr<Led>> getLeds();
    std::shared_ptr<Pot> getRecPot();
    std::shared_ptr<Pot> getVolPot();
    std::shared_ptr<Slider> getSlider();

    std::shared_ptr<HwComponent> getComponentByLabel(const std::string& label);

    Hardware(mpc::Mpc& mpc);

};
}
