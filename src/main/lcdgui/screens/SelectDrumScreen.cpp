#include "SelectDrumScreen.hpp"

#include "Mpc.hpp"
#include "lcdgui/screens/DrumScreen.hpp"

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;

SelectDrumScreen::SelectDrumScreen(Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "select-drum", layerIndex)
{
}

void SelectDrumScreen::open()
{
    const auto drumScreen = mpc.screens->get<ScreenId::DrumScreen>();
    ls.lock()->setFunctionKeysArrangement(
        sequencer::drumBusTypeToDrumIndex(drumScreen->getDrum()));
}

void SelectDrumScreen::function(const int i)
{
    if (i > 3)
    {
        return;
    }

    const auto drumScreen = mpc.screens->get<ScreenId::DrumScreen>();
    drumScreen->setDrum(sequencer::drumBusIndexToDrumBusType(i));

    if (redirectScreen.empty())
    {
        openScreenById(ScreenId::PgmAssignScreen);
    }
    else
    {
        ls.lock()->openScreen(redirectScreen);
        redirectScreen = "";
    }
}
