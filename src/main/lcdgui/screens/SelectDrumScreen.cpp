#include "SelectDrumScreen.hpp"

#include <lcdgui/screens/DrumScreen.hpp>

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;

SelectDrumScreen::SelectDrumScreen(mpc::Mpc& mpc, const int layerIndex) 
	: ScreenComponent(mpc, "select-drum", layerIndex)
{
}

void SelectDrumScreen::open()
{
	auto drumScreen = mpc.screens->get<DrumScreen>("drum");
	ls->setFunctionKeysArrangement(drumScreen->getDrum());
}

void SelectDrumScreen::function(int i)
{
	init();

	if (i > 3)
    {
        return;
    }

	auto drumScreen = mpc.screens->get<DrumScreen>("drum");
	drumScreen->setDrum(i);

	if (redirectScreen.empty())
	{
        openScreen("program-assign");
	}
	else {
        openScreen(redirectScreen);
        redirectScreen = "";
	}
}
