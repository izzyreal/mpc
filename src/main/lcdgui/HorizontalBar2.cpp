#include "HorizontalBar2.hpp"

#include <cmath>

using namespace mpc::lcdgui;

HorizontalBar2::HorizontalBar2(MRECT rect) : Component("horizontal-bar")
{
    setSize(rect.W(), rect.H());
    setLocation(rect.L, rect.T);
}

void HorizontalBar2::setValue(float value)
{
    this->value = value;
    SetDirty();
}

void HorizontalBar2::Draw(std::vector<std::vector<bool>> *pixels)
{
    if (shouldNotDraw(pixels))
    {
        return;
    }

    auto rect = getRect();
    int x = rect.L;
    int y = rect.T;
    int x2 = x + floor(1 + value * w);

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
