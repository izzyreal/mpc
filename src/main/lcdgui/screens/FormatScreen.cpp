#include "FormatScreen.hpp"
#include "Mpc.hpp"

using namespace mpc::lcdgui::screens;

FormatScreen::FormatScreen(Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "format", layerIndex)
{
}

void FormatScreen::function(int i)
{
    switch (i)
    {
        case 0:
            openScreenById(ScreenId::LoadScreen);
            break;
        case 1:
            openScreenById(ScreenId::SaveScreen);
            break;
    }
}
