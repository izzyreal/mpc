#include "SelectMixerDrumScreen.hpp"

#include <lcdgui/screens/DrumScreen.hpp>
#include <lcdgui/screens/MixerScreen.hpp>

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;

SelectMixerDrumScreen::SelectMixerDrumScreen(mpc::Mpc& mpc, const int layerIndex)
	: ScreenComponent(mpc, "select-mixer-drum", layerIndex)
{
}

void SelectMixerDrumScreen::open()
{
	auto drumScreen = mpc.screens->get<DrumScreen>();
	ls->setFunctionKeysArrangement(drumScreen->getDrum());
}

void SelectMixerDrumScreen::function(int i)
{

	if (i < 4)
	{
		auto drumScreen = mpc.screens->get<DrumScreen>();
		drumScreen->setDrum(i);
        mpc.getLayeredScreen()->openScreen<MixerScreen>();
	}
	else if (i == 4)
	{
        mpc.getLayeredScreen()->openScreen<MixerSetupScreen>();
	}
}
