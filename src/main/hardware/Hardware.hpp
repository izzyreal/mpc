#pragma once

#include <Observer.hpp>

#include <vector>
#include <memory>
#include <string>

namespace mpc { class Mpc; }

namespace mpc::hardware
{
class HwComponent;
class TopPanel;
class Button;
class DataWheel;
class Pot;
class Led;
class Slider;
class HwPad;
}

namespace mpc::hardware
{

class PadAndButtonKeyboard;

class Hardware final : public Observable
{

private:
    PadAndButtonKeyboard* padAndButtonKeyboard;
    std::vector<std::string> buttonLabels;
    std::vector<std::shared_ptr<HwComponent>> components;

    std::shared_ptr<TopPanel> topPanel;
    std::vector<std::shared_ptr<HwPad>> pads;
    std::vector<std::shared_ptr<Button>> buttons;
    std::vector<std::shared_ptr<Led>> leds;

    std::shared_ptr<DataWheel> dataWheel;
    std::shared_ptr<Slider> slider;
    std::shared_ptr<Pot> recPot;
    std::shared_ptr<Pot> volPot;

public:
    PadAndButtonKeyboard* getPadAndButtonKeyboard();
    std::shared_ptr<TopPanel> getTopPanel();
    std::vector<std::string>& getButtonLabels();
    std::shared_ptr<HwPad> getPad(int index);
    std::vector<std::shared_ptr<HwPad>>& getPads();
    std::shared_ptr<Button> getButton(std::string label);
    std::shared_ptr<Led> getLed(std::string label);
    std::vector<std::shared_ptr<Led>> getLeds();
    std::shared_ptr<DataWheel> getDataWheel();
    std::shared_ptr<Pot> getRecPot();
    std::shared_ptr<Pot> getVolPot();
    std::shared_ptr<Slider> getSlider();

    std::shared_ptr<HwComponent> getComponentByLabel(const std::string& label);

    Hardware(mpc::Mpc& mpc);

};
}
