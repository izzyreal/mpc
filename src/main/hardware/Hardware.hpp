#pragma once

#include <vector>
#include <memory>
#include <string>

namespace mpc { class Mpc; }

namespace mpc::hardware
{
class TopPanel;
class Led;
}

namespace mpc::hardware
{

class PadAndButtonKeyboard;

class Hardware final
{

private:
    PadAndButtonKeyboard* padAndButtonKeyboard;
    std::shared_ptr<TopPanel> topPanel;
    std::vector<std::shared_ptr<Led>> leds;

public:
    PadAndButtonKeyboard* getPadAndButtonKeyboard();
    std::shared_ptr<TopPanel> getTopPanel();
    std::shared_ptr<Led> getLed(std::string label);
    std::vector<std::shared_ptr<Led>> getLeds();

    Hardware(mpc::Mpc& mpc);

};
}
