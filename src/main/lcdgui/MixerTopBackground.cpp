#include "lcdgui/MixerTopBackground.hpp"

using namespace mpc::lcdgui;

MixerTopBackground::MixerTopBackground(MRECT rect)
    : Component("mixer-top-background")
{
    setSize(rect.W(), rect.H());
    setLocation(rect.L, rect.T);
}

void MixerTopBackground::setColor(bool on)
{
    if (color == on)
    {
        return;
    }

    color = on;
    SetDirty();
}

void MixerTopBackground::Draw(std::vector<std::vector<bool>> *pixels)
{
    if (shouldNotDraw(pixels))
    {
        return;
    }

    const auto rect = getRect();

    for (int i = rect.L; i < rect.R; i++)
    {
        for (int j = rect.T; j < rect.B; j++)
        {
            (*pixels)[i][j] = color;
        }
    }

    Component::Draw(pixels);
}
