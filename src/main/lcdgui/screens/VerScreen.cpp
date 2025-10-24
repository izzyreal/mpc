#include "VerScreen.hpp"

using namespace mpc::lcdgui::screens;

VerScreen::VerScreen(mpc::Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "ver", layerIndex)
{
}

void VerScreen::function(int i)
{

    switch (i)
    {
    case 0:
        mpc.getLayeredScreen()->openScreen<OthersScreen>();
        break;
    case 1:
        mpc.getLayeredScreen()->openScreen<InitScreen>();
        break;
    }
}
