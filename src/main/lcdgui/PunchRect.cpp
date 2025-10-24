#include "PunchRect.hpp"

using namespace mpc::lcdgui;

PunchRect::PunchRect(const std::string &name, MRECT rect)
    : Component(name)
{
    w = rect.W();
    h = rect.H();
    x = rect.L;
    y = rect.T;
}

void PunchRect::Draw(std::vector<std::vector<bool>> *pixels)
{
    if (shouldNotDraw(pixels))
    {
        return;
    }

    for (int x1 = x; x1 < x + w; x1++)
    {
        for (int y1 = y; y1 < y + h; y1++)
        {
            if (x1 == x || x1 == x + w - 1 || y1 == y || y1 == y + h - 1)
            {
                (*pixels)[x1][y1] = true;
            }
            else if (on && ((y1 % 2 == 0 && x1 % 2 == 0) || (y1 % 2 == 1 && x1 % 2 == 1)))
            {
                (*pixels)[x1][y1] = true;
            }
            else
            {
                (*pixels)[x1][y1] = false;
            }
        }
    }

    dirty = false;
}

void PunchRect::setOn(bool newOn)
{
    if (on == newOn)
    {
        return;
    }

    on = newOn;

    dirty = true;
}
