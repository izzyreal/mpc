#include "IgnoreTempoChangeScreen.hpp"

#include "lcdgui/screens/LoadScreen.hpp"

using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui::screens;

IgnoreTempoChangeScreen::IgnoreTempoChangeScreen(Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "ignore-tempo-change", layerIndex)
{
}

void IgnoreTempoChangeScreen::open()
{
    findField("insequence")->setAlignment(Alignment::Centered);
    displayIgnore();
}

void IgnoreTempoChangeScreen::function(const int i)
{
    switch (i)
    {
        case 3:
            openScreenById(ScreenId::SongScreen);
            break;
    }
}

void IgnoreTempoChangeScreen::turnWheel(const int i)
{
    ignore = i > 0;
    displayIgnore();
}

void IgnoreTempoChangeScreen::displayIgnore() const
{
    findField("insequence")->setText(ignore ? "ON" : "OFF");
}

void IgnoreTempoChangeScreen::setIgnore(const bool b)
{
    ignore = b;
}

bool IgnoreTempoChangeScreen::getIgnore() const
{
    return ignore;
}