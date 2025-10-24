#include "FormatScreen.hpp"

using namespace mpc::lcdgui::screens;

FormatScreen::FormatScreen(mpc::Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "format", layerIndex)
{
}

void FormatScreen::function(int i)
{

    switch (i)
    {
    case 0:
        mpc.getLayeredScreen()->openScreen<LoadScreen>();
        break;
    case 1:
        mpc.getLayeredScreen()->openScreen<SaveScreen>();
        break;
    }
}
