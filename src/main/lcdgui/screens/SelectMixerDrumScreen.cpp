#include "SelectMixerDrumScreen.hpp"

#include <ui/sampler/SamplerGui.hpp>
#include <lcdgui/Screens.hpp>
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
		samplerGui->setPrevScreenName(csn);
		ls.lock()->openScreen("mixer");
	}
}
