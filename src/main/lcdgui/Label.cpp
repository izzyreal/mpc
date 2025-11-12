#include "Label.hpp"

using namespace mpc::lcdgui;

Label::Label(Mpc &mpc, const std::string &name, std::string text, int x, int y,
             int width)
    : TextComp(mpc, name)
{
    this->text = text;
    TextComp::setText(text);
    TextComp::setSize(width + 2, 9);
    setLocation(x - 1, y);
    preDrawClearRect.Clear();
    dirty = false;
}
