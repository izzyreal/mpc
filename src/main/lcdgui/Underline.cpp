#include "lcdgui/Underline.hpp"

using namespace mpc::lcdgui;

Underline::Underline() : Component("underline")
{
    states = std::vector<bool>(16);
    Component::setSize(95, 1);
    setLocation(106, 27);
}

void Underline::Draw(std::vector<std::vector<bool>> *pixels)
{
    if (shouldNotDraw(pixels))
    {
        return;
    }

    for (int i = 0; i < 16; i++)
    {
        for (int j = 0; j < 6; j++)
        {
            (*pixels)[j + i * 6 + x][y] = states[i];
        }
    }
    dirty = false;
}

void Underline::setState(int i, bool b)
{
    states[i] = b;
    SetDirty();
}
