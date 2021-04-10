#include "SelectDrumScreen.hpp"

#include <sampler/Program.hpp>

#include <lcdgui/screens/DrumScreen.hpp>

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::controls;
using namespace std;

SelectDrumScreen::SelectDrumScreen(mpc::Mpc& mpc, const int layerIndex) 
	: ScreenComponent(mpc, "select-drum", layerIndex)
{
}

void SelectDrumScreen::open()
{
	auto drumScreen = mpc.screens->get<DrumScreen>("drum");
	ls.lock()->setFunctionKeysArrangement(drumScreen->drum);
}

void SelectDrumScreen::function(int i)
{
	init();

	if (i > 3)
		return;

	auto drumScreen = mpc.screens->get<DrumScreen>("drum");
	drumScreen->drum = i;

	if (mpc.getNote() < 35)
		mpc.setPadAndNote(program.lock()->getPadIndexFromNote(35), 35);

	if (redirectScreen.compare("") != 0)
	{
		openScreen(redirectScreen);
		redirectScreen = "";
	}
	else {
		openScreen("program-assign");
	}
	return;
}
