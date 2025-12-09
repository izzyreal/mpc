#include "HorizontalBar.hpp"

#include <cmath>

using namespace mpc::lcdgui;

HorizontalBar::HorizontalBar(MRECT rect, int value)
    : Component("horizontal-bar")
{
    setSize(rect.W(), rect.H());
    setLocation(rect.L, rect.T);
    this->value = value;
}

void HorizontalBar::setValue(int value)
{
    this->value = value;
    SetDirty();
}

void HorizontalBar::Draw(std::vector<std::vector<bool>> *pixels)
{
    if (shouldNotDraw(pixels))
    {
        return;
    }

    const auto rect = getRect();
    const int x = rect.L;
    const int y = rect.T;
    const int x2 = x + (int)floor((value - 1) / 2.55) + 1;

    for (int i = rect.L; i <= rect.R; i++)
    {
        if (i > 247)
        {
            break;
        }

        for (int j = y; j < y + 5; j++)
        {
            (*pixels)[i][j] = false;
        }
    }

    for (int i = x; i < x2; i++)
    {
        if (i > rect.R)
        {
            break;
        }

        for (int j = y; j < y + 5; j++)
        {
            (*pixels)[i][j] = true;
        }
    }

    dirty = false;
}
