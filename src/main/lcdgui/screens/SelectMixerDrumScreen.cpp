#include "SelectMixerDrumScreen.hpp"

#include <lcdgui/screens/DrumScreen.hpp>

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace std;

SelectMixerDrumScreen::SelectMixerDrumScreen(const int layerIndex)
	: ScreenComponent("select-mixer-drum", layerIndex)
{
}

void SelectMixerDrumScreen::function(int i)
{
	init();

	if (i < 4)
	{
		auto drumScreen = dynamic_pointer_cast<DrumScreen>(Screens::getScreenComponent("drum"));
		drumScreen->setDrum(i);
		mpc.setPreviousSamplerScreenName(currentScreenName);
		ls.lock()->openScreen("mixer");
	}
}
