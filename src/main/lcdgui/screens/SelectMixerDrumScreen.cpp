#include "SelectMixerDrumScreen.hpp"

#include "Mpc.hpp"
#include "lcdgui/screens/DrumScreen.hpp"
#include "lcdgui/screens/MixerScreen.hpp"

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;

SelectMixerDrumScreen::SelectMixerDrumScreen(mpc::Mpc &mpc,
                                             const int layerIndex)
    : ScreenComponent(mpc, "select-mixer-drum", layerIndex)
{
}

void SelectMixerDrumScreen::open()
{
    auto drumScreen = mpc.screens->get<ScreenId::DrumScreen>();
    ls->setFunctionKeysArrangement(drumScreen->getDrum());
}

void SelectMixerDrumScreen::function(int i)
{

    if (i < 4)
    {
        auto drumScreen = mpc.screens->get<ScreenId::DrumScreen>();
        drumScreen->setDrum(i);
        openScreenById(ScreenId::MixerScreen);
    }
    else if (i == 4)
    {
        openScreenById(ScreenId::MixerSetupScreen);
    }
}
