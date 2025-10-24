#include "EventRowParameters.hpp"

using namespace mpc::lcdgui;

EventRowParameters::EventRowParameters(MRECT rect)
    : Component("event-row-parameters")
{
    setSize(rect.W(), rect.H());
    setLocation(rect.L, rect.T);
}

void EventRowParameters::setColor(bool on)
{
    color = on;
    SetDirty();
}

void EventRowParameters::Draw(std::vector<std::vector<bool>> *pixels)
{
    if (shouldNotDraw(pixels))
    {
        return;
    }

    SetDirty(); // Whenever we draw this component, we mark the children as dirty as well

    auto rect = getRect();

    for (int i = 0; i < rect.W(); i++)
    {
        for (int j = 0; j < rect.H(); j++)
        {
            pixels->at(i + rect.L).at(j + rect.T) = color;
        }
    }

    Component::Draw(pixels);
}
