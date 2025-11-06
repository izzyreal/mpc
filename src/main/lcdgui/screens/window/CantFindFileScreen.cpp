#include "CantFindFileScreen.hpp"

using namespace mpc::lcdgui::screens::window;

CantFindFileScreen::CantFindFileScreen(Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "cant-find-file", layerIndex)
{
}

void CantFindFileScreen::open()
{
    findField("file")->setText(fileName);
}

void CantFindFileScreen::function(const int i)
{

    switch (i)
    {
        case 1:
            skipAll = true;
            waitingForUser = false;
            break;
        case 2:
            waitingForUser = false;
            break;
    }
}
