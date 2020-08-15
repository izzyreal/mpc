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
	auto drumScreen = dynamic_pointer_cast<DrumScreen>(mpc.screens->getScreenComponent("drum"));
	ls.lock()->setFunctionKeysArrangement(drumScreen->drum);
}

void SelectMixerDrumScreen::function(int i)
{
	init();

	if (i < 4)
	{
		auto drumScreen = dynamic_pointer_cast<DrumScreen>(mpc.screens->getScreenComponent("drum"));
		drumScreen->drum = i;
		ls.lock()->openScreen("mixer");
	}
	else if (i == 4)
	{
		ls.lock()->openScreen("mixer-setup");
	}
}
