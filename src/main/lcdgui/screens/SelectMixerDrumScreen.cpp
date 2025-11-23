#include "SelectMixerDrumScreen.hpp"

#include "Mpc.hpp"
#include "lcdgui/screens/DrumScreen.hpp"

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;

SelectMixerDrumScreen::SelectMixerDrumScreen(Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "select-mixer-drum", layerIndex)
{
}

void SelectMixerDrumScreen::open()
{
    const auto drumScreen = mpc.screens->get<ScreenId::DrumScreen>();
    ls.lock()->setFunctionKeysArrangement(
        sequencer::drumBusTypeToDrumIndex(drumScreen->getDrum()));
}

void SelectMixerDrumScreen::function(const int i)
{
    if (i < 4)
    {
        const auto drumScreen = mpc.screens->get<ScreenId::DrumScreen>();
        drumScreen->setDrum(sequencer::drumBusIndexToDrumBusType(i));
        openScreenById(ScreenId::MixerScreen);
    }
    else if (i == 4)
    {
        openScreenById(ScreenId::MixerSetupScreen);
    }
}
