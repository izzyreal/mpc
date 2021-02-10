#include "SelectMixerDrumScreen.hpp"

#include <lcdgui/screens/DrumScreen.hpp>
#include <lcdgui/screens/MixerScreen.hpp>

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace std;

SelectMixerDrumScreen::SelectMixerDrumScreen(mpc::Mpc& mpc, const int layerIndex)
	: ScreenComponent(mpc, "select-mixer-drum", layerIndex)
{
}

void SelectMixerDrumScreen::open()
{
	auto drumScreen = mpc.screens->get<DrumScreen>("drum");
	ls.lock()->setFunctionKeysArrangement(drumScreen->drum);
}

void SelectMixerDrumScreen::function(int i)
{
	init();

	if (i < 4)
	{
		auto drumScreen = mpc.screens->get<DrumScreen>("drum");
		drumScreen->drum = i;
		openScreen("mixer");
	}
	else if (i == 4)
	{
		openScreen("mixer-setup");
	}
}
