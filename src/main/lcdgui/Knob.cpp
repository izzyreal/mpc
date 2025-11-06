#include "lcdgui/Knob.hpp"

#include <Util.hpp>
#include "Bressenham.hpp"

using namespace mpc::lcdgui;

Knob::Knob(MRECT rect) : Component("knob")
{
    Component::setSize(rect.W(), rect.H());
    setLocation(rect.L, rect.T);
}

void Knob::setValue(int newValue)
{
    value = newValue;
    SetDirty();
}

void Knob::setColor(bool on)
{
    color = on;
    SetDirty();
}

void Knob::Draw(std::vector<std::vector<bool>> *pixels)
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
            (*pixels)[i][j] = !color;
        }
    }

    LcdBitmap lines;
    lines.emplace_back(Bressenham::Line(0, 3, 0, 7));
    lines.emplace_back(Bressenham::Line(0, 7, 3, 10));
    lines.emplace_back(Bressenham::Line(3, 10, 7, 10));
    lines.emplace_back(Bressenham::Line(7, 10, 10, 7));
    lines.emplace_back(Bressenham::Line(10, 7, 10, 3));
    lines.emplace_back(Bressenham::Line(10, 3, 7, 0));
    lines.emplace_back(Bressenham::Line(7, 0, 3, 0));
    lines.emplace_back(Bressenham::Line(3, 0, 0, 3));

    const int angle = (int)((value * 3.1) - 245);
    const float radius = 4.95f;
    const int radiusInt = (int)(radius);
    const float angleToRadians = ((float)(angle) / 180.0) * 3.14159265;
    const int x0 = (int)(radius * cos(angleToRadians));
    const int y0 = (int)(radius * sin(angleToRadians));
    lines.emplace_back(
        Bressenham::Line(5, 5, x0 + radiusInt + 1, y0 + radiusInt + 1));

    const std::vector<bool> colors(lines.size(), color);

    const std::vector<int> offsetxy{rect.L, rect.T};
    mpc::Util::drawLines(*pixels, lines, colors, offsetxy);
    dirty = false;
}
