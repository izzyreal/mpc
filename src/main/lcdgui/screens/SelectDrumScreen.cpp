#include "SelectDrumScreen.hpp"

#include <sampler/Program.hpp>

#include <lcdgui/screens/DrumScreen.hpp>

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace std;

SelectDrumScreen::SelectDrumScreen(const int layerIndex) 
	: ScreenComponent("select-drum", layerIndex)
{
}

void SelectDrumScreen::open()
{
	auto drumScreen = dynamic_pointer_cast<DrumScreen>(Screens::getScreenComponent("drum"));
	ls.lock()->setFunctionKeysArrangement(drumScreen->drum);
}

void SelectDrumScreen::function(int i)
{
	init();

	if (i >= 4)
	{
		return;
	}

	auto drumScreen = dynamic_pointer_cast<DrumScreen>(Screens::getScreenComponent("drum"));
	drumScreen->drum = i;

	auto previousScreenName = ls.lock()->getPreviousScreenName();
	auto name = string("program-assign");

	if (previousScreenName.compare("program-params") == 0 || previousScreenName.compare("drum") == 0 || previousScreenName.compare("purge") == 0)
	{
		name = previousScreenName;
	}

	if (mpc.getNote() < 35)
	{
		mpc.setPadAndNote(program.lock()->getPadIndexFromNote(35), 35);
	}

	ls.lock()->openScreen(name);
	return;
}