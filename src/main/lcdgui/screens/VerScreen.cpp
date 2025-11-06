#include "VerScreen.hpp"

using namespace mpc::lcdgui::screens;

VerScreen::VerScreen(Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "ver", layerIndex)
{
}

void VerScreen::function(int i)
{

    switch (i)
    {
        case 0:
            openScreenById(ScreenId::OthersScreen);
            break;
        case 1:
            openScreenById(ScreenId::InitScreen);
            break;
    }
}
