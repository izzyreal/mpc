#include "SelectDrumScreen.hpp"

#include "Mpc.hpp"
#include "lcdgui/screens/DrumScreen.hpp"

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;

SelectDrumScreen::SelectDrumScreen(mpc::Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "select-drum", layerIndex)
{
}

void SelectDrumScreen::open()
{
    const auto drumScreen = mpc.screens->get<ScreenId::DrumScreen>();
    ls->setFunctionKeysArrangement(drumScreen->getDrum());
}

void SelectDrumScreen::function(int i)
{

    if (i > 3)
    {
        return;
    }

    const auto drumScreen = mpc.screens->get<ScreenId::DrumScreen>();
    drumScreen->setDrum(i);

    if (redirectScreen.empty())
    {
        openScreenById(ScreenId::PgmAssignScreen);
    }
    else
    {
        ls->openScreen(redirectScreen);
        redirectScreen = "";
    }
}
